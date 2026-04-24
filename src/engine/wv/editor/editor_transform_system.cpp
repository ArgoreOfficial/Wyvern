#include "editor_transform_system.h"

#include <wv/application.h>
#include <wv/entity/world.h>
#include <wv/input/input_system.h>

#include <wv/math/geometry.h>

#include <wv/editor/editor_camera_system.h>
#include <wv/editor/editor_interface_system.h>

#include <wv/components/camera_component.h>

#include <wv/rendering/renderer.h>

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

	if ( m_editorCamera == nullptr )
		m_editorCamera = world->getSystem<EditorInterfaceSystem>()->getEditorCamera();
	
	if ( !m_editorCamera )
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
	{
		justStartedTransform = true;
		
		CameraComponent& cameraComp = getWorld()->getComponent<CameraComponent>( m_editorCamera );

		Vector2i mousePos = inputSystem->getMousePosition();
		m_mousePosWhenTransformStart = mousePos;
		cameraComp.screenToWorldRay( mousePos.x, mousePos.y, 0.01f, 1.0f, m_transformLineStart, m_transformLineEnd );
	}

	if ( m_transformMode != EditTransformMode_None )
	{
		Vector2f mouseMove = (Vector2f)inputSystem->getMouseMotion();

		if ( m_leftShiftState )
			mouseMove *= 0.1f;

		m_accumulatedMouseMove += mouseMove;
	}

	centreOfMass /= (float)numSelectedObjects;
	m_editCenterOfMass = centreOfMass;

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

				switch ( m_transformMode )
				{
				case EditTransformMode_Translate: translateObject( entity, editorObject, m_editCenterOfMass, 0.001f ); break;
				case EditTransformMode_Rotate:    rotateObject( entity, editorObject, m_editCenterOfMass, 0.001f ); break;
				case EditTransformMode_Scale:     scaleObject( entity, editorObject, m_editCenterOfMass, 0.25f ); break;
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
	if ( m_transformMode != EditTransformMode_None )
	{
		Renderer* renderer = getApp()->getRenderer();

		switch ( m_lockMovementAxis )
		{
		case 0: renderer->addDebugLine( m_editCenterOfMass + Vector3f{ -100.0f, 0.0f, 0.0f }, m_editCenterOfMass + Vector3f{ 100.0f, 0.0f, 0.0f } ); break;
		case 1: renderer->addDebugLine( m_editCenterOfMass + Vector3f{ 0.0f, -100.0f, 0.0f }, m_editCenterOfMass + Vector3f{ 0.0f, 100.0f, 0.0f } ); break;
		case 2: renderer->addDebugLine( m_editCenterOfMass + Vector3f{ 0.0f, 0.0f, -100.0f }, m_editCenterOfMass + Vector3f{ 0.0f, 0.0f, 100.0f } ); break;
		}
	}
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
	if ( !isTransforming || ( m_transformMode != _mode ) )
	{
		m_accumulatedMouseMove = {};
		m_lockMovementAxis = -1;
	}

	m_transformMode = _mode;
}

void wv::EditorTransformSystem::translateObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength )
{
	Transform& tfm = _entity->getTransform();
	CameraComponent& cameraComp = getWorld()->getComponent<CameraComponent>( m_editorCamera );

	Vector3f newPosition = _editorComponent.translateStart;

	float dist = ( _com - m_editorCamera->getTransform().position ).length();
	_strength *= dist;

	if ( m_lockMovementAxis == -1 )
	{
		Vector3f right = m_editorCamera->getTransform().right();
		Vector3f up = m_editorCamera->getTransform().up();

		newPosition += ( right * m_accumulatedMouseMove.x + up * -m_accumulatedMouseMove.y ) * _strength;
	}
	else
	{
		Vector3f planeDirection{};
		
		switch ( m_lockMovementAxis )
		{
		case 0: planeDirection = { 0.0f, 1.0f, 0.0f }; break;

		case 1:
		{
			Vector3f absDir = m_editorCamera->getTransform().forward();
			absDir.x = Math::abs( absDir.x );
			absDir.z = Math::abs( absDir.z );

			if( absDir.x > absDir.z ) planeDirection = { 1.0f, 0.0f, 0.0f };
			if( absDir.z > absDir.x ) planeDirection = { 0.0f, 0.0f, 1.0f };

		} break;

		case 2: planeDirection = { 0.0f, 1.0f, 0.0f }; break;
		}

		Vector3f editStartPoint, editCurrentPoint;
		Vector3f transformLineDir = ( m_transformLineEnd - m_transformLineStart ).normalized();

		if ( Math::linePlaneIntersection( m_transformLineStart, transformLineDir, _com, planeDirection, &editStartPoint ) )
		{
			Vector2i mousePos = m_mousePosWhenTransformStart + m_accumulatedMouseMove;
			Vector3f lineStart, lineEnd;

			cameraComp.screenToWorldRay( mousePos.x, mousePos.y, 0.01f, 1.0f, lineStart, lineEnd );

			Vector3f lineDir = ( lineEnd - lineStart ).normalized();

			if ( Math::linePlaneIntersection( lineStart, lineDir, _com, planeDirection, &editCurrentPoint ) )
			{
				switch ( m_lockMovementAxis )
				{
				case 0: newPosition.x += editCurrentPoint.x - editStartPoint.x; break;
				case 1: newPosition.y += editCurrentPoint.y - editStartPoint.y; break;
				case 2: newPosition.z += editCurrentPoint.z - editStartPoint.z; break;
				}
			}
		}
	}

	
	tfm.position = newPosition;
	tfm.rotation = _editorComponent.rotateStart;
	tfm.scale = _editorComponent.scaleStart;
}

void wv::EditorTransformSystem::rotateObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength )
{
	Transform& tfm = _entity->getTransform();

	Rotorf rotor{};
	float radians = -m_accumulatedMouseMove.x * _strength;

	switch ( m_lockMovementAxis )
	{
	case -1:
	{
		Vector3f bvPlane = wv::Math::wedge( 
			m_editorCamera->getTransform().right(), 
			m_editorCamera->getTransform().up() 
		);

		rotor = Rotorf{ bvPlane, radians }; 
	} break;

	case 0: rotor = Rotorf{ { 0.0,  0.0, 1.0 }, radians }; break;
	case 1: rotor = Rotorf{ { 0.0, -1.0, 0.0 }, radians }; break;
	case 2: rotor = Rotorf{ { 1.0,  0.0, 0.0 }, radians }; break;
	}

	tfm.position = _editorComponent.translateStart;
	tfm.rotation = rotor * _editorComponent.rotateStart;
	tfm.scale = _editorComponent.scaleStart;
}

void wv::EditorTransformSystem::scaleObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength )
{
	InputSystem* inputSystem = getApp()->getInputSystem();
	Transform& tfm = _entity->getTransform();
	CameraComponent& cameraComp = getWorld()->getComponent<CameraComponent>( m_editorCamera );

	Vector2i mousePos  = m_mousePosWhenTransformStart + m_accumulatedMouseMove;
	Vector3f lineStart, lineEnd;

	cameraComp.screenToWorldRay( mousePos.x, mousePos.y, 0.01f, 1.0f, lineStart, lineEnd );

	const Vector3f editOriginPoint  = Math::lineClosestPointClamped( m_transformLineStart, m_transformLineEnd, _com );
	const Vector3f editCurrentPoint = wv::Math::lineClosestPointClamped( lineStart, lineEnd, _com );
	const float editOriginDistance  = ( editOriginPoint  - _com ).length();
	const float editCurrentDistance = ( editCurrentPoint - _com ).length();
	const float scale = editCurrentDistance / editOriginDistance;

	Vector3f newScale = _editorComponent.scaleStart;

	switch ( m_lockMovementAxis )
	{
	case -1: newScale   *= scale; break;
	case 0:  newScale.x *= scale; break;
	case 1:  newScale.y *= scale; break;
	case 2:  newScale.z *= scale; break;
	}

	tfm.position = _editorComponent.translateStart;
	tfm.rotation = _editorComponent.rotateStart;
	tfm.scale = newScale;
}
