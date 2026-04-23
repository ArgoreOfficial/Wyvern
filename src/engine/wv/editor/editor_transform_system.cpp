#include "editor_transform_system.h"

#include <wv/application.h>
#include <wv/entity/world.h>
#include <wv/input/input_system.h>

#include <wv/editor/editor_camera_system.h>
#include <wv/systems/camera_manager_system.h>

void wv::EditorTransformSystem::onInitialize()
{
	setEditorRenderEnabled( true );

	m_editorActionGroup = updateContext->inputSystem->getActionGroup( "Editor" );

	m_editTranslateActionID = m_editorActionGroup->getTriggerActionID( "Translate" );
	m_editRotateActionID = m_editorActionGroup->getTriggerActionID( "Rotate" );
	m_editScaleActionID = m_editorActionGroup->getTriggerActionID( "Scale" );

	m_lockAxisXActionID = m_editorActionGroup->getTriggerActionID( "LockAxisX" );
	m_lockAxisYActionID = m_editorActionGroup->getTriggerActionID( "LockAxisY" );
	m_lockAxisZActionID = m_editorActionGroup->getTriggerActionID( "LockAxisZ" );

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

	// Get number of selected objects and centre of mass

	Vector3f centreOfMass{};
	size_t numSelectedObjects = 0;
	for ( Archetype* arch : getArchetypes() )
	{
		auto& comps = arch->getComponents<EditorObjectComponent>();
		auto& ents = arch->getEntities();

		for ( size_t i = 0; i < arch->getNumEntities(); i++ )
		{
			if ( !comps[ i ].selected )
				continue;

			centreOfMass += ents[ i ]->getTransform().position;
			numSelectedObjects++;
		}
	}

	// No object selected, system does not need to run
	if ( numSelectedObjects == 0 )
		return;

	// Update input

	bool justStartedTransform = false;
	bool cancelledTransform = false;

	EditTransformMode prevTransformMode = m_transformMode;
	
	for ( ActionEvent& ae : updateContext->actionEventQueue )
	{
		if ( ae.actionID == m_editTranslateActionID && ae.action.trigger->getValue() )
			setEditTransformMode( EditTransformMode_Translate );
		
		if ( ae.actionID == m_editRotateActionID && ae.action.trigger->getValue() )
			setEditTransformMode( EditTransformMode_Rotate );

		if ( ae.actionID == m_editScaleActionID && ae.action.trigger->getValue() )
			setEditTransformMode( EditTransformMode_Scale );

		// Left click, accept transform
		if ( ae.actionID == m_mouseLeftActionID && m_transformMode != EditTransformMode_None )
			setEditTransformMode( EditTransformMode_None );
		
		// Right click, cancelt transform
		if ( ae.actionID == m_mouseRightActionID && m_transformMode != EditTransformMode_None )
		{
			setEditTransformMode( EditTransformMode_None );
			cancelledTransform = true;
		}

		if ( ae.actionID == m_lockAxisXActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 0;
		if ( ae.actionID == m_lockAxisYActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 1;
		if ( ae.actionID == m_lockAxisZActionID && ae.action.trigger->getValue() ) m_lockMovementAxis = 2;

		if ( ae.actionID == m_shiftActionID ) m_leftShiftState = ae.action.value->getValue() > 0.001f;
	}

	if ( prevTransformMode == EditTransformMode_None && m_transformMode != EditTransformMode_None )
		justStartedTransform = true;

	if ( m_transformMode != EditTransformMode_None )
	{
		Vector2f mouseMove = (Vector2f)inputSystem->getMouseMotion();

		if ( m_leftShiftState )
			mouseMove *= 0.1f;

		m_accumulatedMouseMove += mouseMove;
	}

	centreOfMass /= (float)numSelectedObjects;

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
				{
					editorObject.translateStart = tfm.position;
					editorObject.rotateStart = tfm.rotation;
					editorObject.scaleStart = tfm.scale;
				}

				float strength = 0.001f;

				switch ( m_transformMode )
				{
				case EditTransformMode_Translate: translateObject( entity, editorObject, centreOfMass, strength ); break;
				case EditTransformMode_Rotate:    rotateObject( entity, editorObject, centreOfMass, strength ); break;
				case EditTransformMode_Scale:     scaleObject( entity, editorObject, centreOfMass, strength ); break;
				}

				if ( cancelledTransform )
				{
					tfm.position = editorObject.translateStart;
					tfm.rotation = editorObject.rotateStart;
					tfm.scale    = editorObject.scaleStart;
				}
			}
		}
	}
}

void wv::EditorTransformSystem::onEditorRender()
{
}


void wv::EditorTransformSystem::setEditTransformMode( EditTransformMode _mode )
{
	if ( m_transformMode == _mode )
		return;

	EditorCameraSystem* cameraSystem = getWorld()->getSystem<EditorCameraSystem>();

	bool isTransforming = _mode != EditTransformMode_None;

	if ( cameraSystem->isCameraLocked() )
	{
		if ( isTransforming ) // do not transform if camera is locked
			return;
	}
	else
		getApp()->setCursorLock( isTransforming );

	// if no longer transforming, or if we just started transforming
	if ( !isTransforming || ( m_transformMode != isTransforming ) )
	{
		m_accumulatedMouseMove = {};
		m_lockMovementAxis = -1;
	}

	m_transformMode = _mode;
}

void wv::EditorTransformSystem::translateObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength )
{
	CameraManagerSystem* cameraManager = getWorld()->getSystem<CameraManagerSystem>();
	Entity* camera = cameraManager->getCameraOverride();
	Transform& tfm = _entity->getTransform();
	Vector3f move{};

	float dist = ( _com - camera->getTransform().position ).length();
	_strength *= dist;

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

	tfm.position = _editorComponent.translateStart + move * _strength;
	tfm.rotation = _editorComponent.rotateStart;
	tfm.scale = _editorComponent.scaleStart;
}

void wv::EditorTransformSystem::rotateObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength )
{
	CameraManagerSystem* cameraManager = getWorld()->getSystem<CameraManagerSystem>();
	Entity* camera = cameraManager->getCameraOverride();
	Transform& tfm = _entity->getTransform();
	
	Vector3f bvPlane = wv::Math::wedge( camera->getTransform().right(), camera->getTransform().up() );
	Rotorf rotor{ bvPlane, -m_accumulatedMouseMove.x * _strength };

	tfm.position = _editorComponent.translateStart;
	tfm.rotation = rotor * _editorComponent.rotateStart;
	tfm.scale = _editorComponent.scaleStart;
}

void wv::EditorTransformSystem::scaleObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength )
{
	Transform& tfm = _entity->getTransform();

	float scale = 1 + ( m_accumulatedMouseMove.length() * _strength );

	tfm.position = _editorComponent.translateStart;
	tfm.rotation = _editorComponent.rotateStart;
	tfm.scale = _editorComponent.scaleStart * scale;
}
