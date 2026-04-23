#include "editor_transform_system.h"

#include <wv/application.h>
#include <wv/entity/world.h>
#include <wv/input/input_system.h>

#include <wv/systems/camera_manager_system.h>

void wv::EditorTransformSystem::onInitialize()
{
	setEditorRenderEnabled( true );

	m_editorActionGroup = updateContext->inputSystem->getActionGroup( "Editor" );

	m_moveObjectActionID = m_editorActionGroup->getTriggerActionID( "MoveObject" );
	m_moveObjectXActionID = m_editorActionGroup->getTriggerActionID( "MoveObjectX" );
	m_moveObjectYActionID = m_editorActionGroup->getTriggerActionID( "MoveObjectY" );
	m_moveObjectZActionID = m_editorActionGroup->getTriggerActionID( "MoveObjectZ" );

	m_mouseLeftActionID = m_editorActionGroup->getTriggerActionID( "MouseLeft" );
	m_mouseRightActionID = m_editorActionGroup->getTriggerActionID( "MouseRight" );

	m_shiftActionID = m_editorActionGroup->getValueActionID( "ShiftLeft" );

}

void wv::EditorTransformSystem::onUpdate()
{
	Application* app = getApp();
	InputSystem* inputSystem = app->getInputSystem();
	World* world = app->getWorld();

	CameraManagerSystem* cameraManager = world->getSystem<CameraManagerSystem>();
	Entity* camera = cameraManager->getCameraOverride();

	if ( !camera )
		return;

	m_selectedEntity = nullptr;

	for ( Archetype* arch : getArchetypes() )
	{
		auto& comps = arch->getComponents<EditorObjectComponent>();
		auto& ents = arch->getEntities();

		for ( size_t i = 0; i < arch->getNumEntities(); i++ )
			if ( comps[ i ].selected )
				m_selectedEntity = ents[ i ];
	}

	if ( !m_selectedEntity )
		return;

	for ( ActionEvent& ae : updateContext->actionEventQueue )
	{
		if ( ae.actionID == m_moveObjectActionID && ae.action.trigger->getValue() )
		{
			if ( m_isMovingObject )
				endMoveObject( true );
			else
				beginMoveObject();
		}

		if ( ae.actionID == m_mouseLeftActionID )
			if ( m_isMovingObject ) endMoveObject( true );

		if ( ae.actionID == m_mouseRightActionID )
			if ( m_isMovingObject ) endMoveObject( false );

		if ( ae.actionID == m_moveObjectXActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 0;
		if ( ae.actionID == m_moveObjectYActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 1;
		if ( ae.actionID == m_moveObjectZActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 2;

		if ( ae.actionID == m_shiftActionID ) m_leftShiftState = ae.action.value->getValue() > 0.001f;
	}

	if ( m_selectedEntity && m_isMovingObject )
	{
		Transform& tfm = m_selectedEntity->getTransform();
		float dist = ( tfm.position - camera->getTransform().position ).length();

		Vector2f mouseMove = (Vector2f)inputSystem->getMouseMotion() / 1000.0f * dist;

		if ( m_leftShiftState )
			mouseMove *= 0.1f;

		m_accumulatedMouseMove += mouseMove;
		switch ( m_lockMovementAxis )
		{
		case -1:
		{
			Vector3f right = camera->getTransform().right();
			Vector3f up = camera->getTransform().up();

			tfm.position = m_moveObjectStartPosition + ( right * m_accumulatedMouseMove.x + up * -m_accumulatedMouseMove.y );
		} break;

		case 0:
			tfm.position = m_moveObjectStartPosition + Vector3f{ m_accumulatedMouseMove.x, 0.0f, 0.0f };
			break;

		case 1:
			tfm.position = m_moveObjectStartPosition + Vector3f{ 0.0f, -m_accumulatedMouseMove.y, 0.0f };
			break;

		case 2:
			tfm.position = m_moveObjectStartPosition + Vector3f{ 0.0f, 0.0f, -m_accumulatedMouseMove.y };
			break;
		}
	}
}

void wv::EditorTransformSystem::onEditorRender()
{
}


void wv::EditorTransformSystem::beginMoveObject()
{
	if ( m_isMovingObject )
		return;

	Application* app = getApp();
	m_isMovingObject = true;

	app->setCursorLock( true );
	m_moveObjectStartPosition = m_selectedEntity->getTransform().position;
	m_lockMovementAxis = -1;
	m_accumulatedMouseMove = {};
}

void wv::EditorTransformSystem::endMoveObject( bool _confirm )
{
	if ( !m_isMovingObject )
		return;

	Application* app = getApp();
	m_isMovingObject = false;

	app->setCursorLock( false );

	if ( !_confirm )
		m_selectedEntity->getTransform().position = m_moveObjectStartPosition;
}
