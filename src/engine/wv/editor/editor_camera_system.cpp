#include "editor_camera_system.h"

#include <wv/application.h>
#include <wv/entity/world.h>
#include <wv/input/input_system.h>
#include <wv/components/camera_component.h>

void wv::EditorCameraSystem::configure( ArchetypeConfig& _config )
{
	m_debugName = "EditorCameraSystem";

	setUpdateMode( UpdateMode_editor );

	_config.addComponentType<CameraComponent>();
	_config.addComponentType<EditorCameraComponent>();
}

void wv::EditorCameraSystem::onInitialize()
{
	m_editorActionGroup = updateContext->inputSystem->getActionGroup( "Editor" );
	m_moveActionID = m_editorActionGroup->getAxisActionID( "Move" );
}

void wv::EditorCameraSystem::onUpdate()
{
	Application* app = getApp();
	InputSystem* inputSystem = app->getInputSystem();

	if ( inputSystem->getMouseButtonDown( MOUSE_BUTTON_RIGHT ) )
	{
		m_editorActionGroup->enable();
		app->setCursorLock( true );
	}
	else if ( inputSystem->getMouseButtonUp( MOUSE_BUTTON_RIGHT ) )
	{
		m_editorActionGroup->disable();
		app->setCursorLock( false );
	}

	for ( auto& ae : updateContext->actionEventQueue )
		if ( ae.actionID == m_moveActionID )
			m_moveInput = ae.action.axis->getValue();
	
	const float friction = 10.f;
	const float frictionDecay = 1 / ( 1 + ( deltaTime * friction ) );

	bool isLockedIn = inputSystem->getMouseButtonState( MOUSE_BUTTON_RIGHT );

	if ( isLockedIn )
	{
		float speedChange = 1.0f;
		
		     if ( m_moveSpeed > 10.0f  ) speedChange = 5.0f;
		else if ( m_moveSpeed > 50.0f  ) speedChange = 25.0f;
		else if ( m_moveSpeed > 100.0f ) speedChange = 50.0f;
		
		m_moveSpeed += (float)inputSystem->getMouseScroll() * speedChange;

		if ( m_moveSpeed < 0.2f )
			m_moveSpeed = 0.2f;

		if ( inputSystem->getMouseScroll() != 0 )
			Debug::Print( "Move Speed: %f\n", m_moveSpeed );
	}

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& cameras = archetype->getComponents<CameraComponent>();
		auto& editorComps = archetype->getComponents<EditorCameraComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			CameraComponent&       cameraComp = cameras[ i ];
			EditorCameraComponent& editorComp = editorComps[ i ];
			Entity* entity = entities[ i ];

			editorComp.look = {};
			editorComp.move *= frictionDecay;

			if ( !cameraComp.active )
				continue;

			if ( isLockedIn )
			{
				Vector2i motion = inputSystem->getMouseMotion();
				editorComp.look.x += 0.15f * (float)motion.x;
				editorComp.look.y += 0.15f * (float)motion.y;

				editorComp.move += m_moveInput * deltaTime * m_moveSpeed;

				if ( editorComp.move.length() > m_moveSpeed )
					editorComp.move.normalize( m_moveSpeed );
			}
			
			wv::Transform& transform = entity->getTransform();

			transform.rotation.rotate( Vector3f{ -editorComp.look.y, 0.0f, 0.0f } );
			transform.rotation.rotate( Vector3f{ 0.0f, -editorComp.look.x, 0.0f }, RotateSpace_world );

			wv::Vector3f moveDirection = 
				transform.forward() * editorComp.move.y + 
				transform.right() * editorComp.move.x;

			transform.position += moveDirection * deltaTime;
		}
	}
}
