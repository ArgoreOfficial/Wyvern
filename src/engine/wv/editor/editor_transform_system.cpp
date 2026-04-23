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
	InputSystem* inputSystem = getApp()->getInputSystem();
	World* world = getWorld();

	CameraManagerSystem* cameraManager = world->getSystem<CameraManagerSystem>();
	Entity* camera = cameraManager->getCameraOverride();

	if ( !camera )
		return;

	bool justStartedTransform = false;
	bool cancelledTransform = false;

	for ( ActionEvent& ae : updateContext->actionEventQueue )
	{
		if ( ae.actionID == m_moveObjectActionID && ae.action.trigger->getValue() )
		{
			if ( m_isMovingObject )
				setIsMoveObject( false );
			else
			{
				justStartedTransform = true;
				setIsMoveObject( true );
			}
		}

		if ( ae.actionID == m_mouseLeftActionID && m_isMovingObject )
			setIsMoveObject( false );
			
		if ( ae.actionID == m_mouseRightActionID && m_isMovingObject )
		{
			setIsMoveObject( false );
			cancelledTransform = true;
		}

		if ( ae.actionID == m_moveObjectXActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 0;
		if ( ae.actionID == m_moveObjectYActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 1;
		if ( ae.actionID == m_moveObjectZActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 2;

		if ( ae.actionID == m_shiftActionID ) m_leftShiftState = ae.action.value->getValue() > 0.001f;
	}

	if ( m_isMovingObject )
	{
		Vector2f mouseMove = (Vector2f)inputSystem->getMouseMotion();

		if ( m_leftShiftState )
			mouseMove *= 0.1f;

		m_accumulatedMouseMove += mouseMove;
	}

	for ( Archetype* arch : getArchetypes() )
	{
		auto& comps = arch->getComponents<EditorObjectComponent>();
		auto& ents = arch->getEntities();

		for ( size_t i = 0; i < arch->getNumEntities(); i++ )
		{
			EditorObjectComponent& editorObject = comps[ i ];
			Entity* entity = ents[ i ];

			if ( editorObject.selected )
			{
				Transform& tfm = entity->getTransform();

				if ( justStartedTransform )
					editorObject.translateStart = tfm.position;
				
				if ( m_isMovingObject )
				{
					float dist = ( tfm.position - camera->getTransform().position ).length();

					Vector3f move{};

					switch ( m_lockMovementAxis )
					{
					case -1:
					{
						Vector3f right = camera->getTransform().right();
						Vector3f up = camera->getTransform().up();

						move = ( right * m_accumulatedMouseMove.x + up * -m_accumulatedMouseMove.y );
					} break;

					case 0: move = Vector3f{ m_accumulatedMouseMove.x,  0.0f, 0.0f }; break;
					case 1: move = Vector3f{ 0.0f, -m_accumulatedMouseMove.y, 0.0f }; break;
					case 2: move = Vector3f{ 0.0f, 0.0f, -m_accumulatedMouseMove.y }; break;
					}

					tfm.position = editorObject.translateStart + ( move / 1000.0f * dist );
				}

				if ( cancelledTransform )
					tfm.position = editorObject.translateStart;
			}
		}
	}
}

void wv::EditorTransformSystem::onEditorRender()
{
}


void wv::EditorTransformSystem::setIsMoveObject( bool _moving )
{
	getApp()->setCursorLock( _moving );

	m_isMovingObject = _moving;
	m_lockMovementAxis = -1;
	m_accumulatedMouseMove = {};
}