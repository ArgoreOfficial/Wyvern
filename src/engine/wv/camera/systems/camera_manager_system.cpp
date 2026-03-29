#include "camera_manager_system.h"

#include <wv/application.h>
#include <wv/camera/view_volume.h>
#include <wv/camera/components/camera_component.h>
#include <wv/camera/components/orbit_camera_component.h>

#include <wv/input/components/player_input_component.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/rendering/viewport.h>

#include <wv/debug/log.h>
#include <wv/input/input_system.h>
#include <wv/event/event_manager.h>

wv::CameraManagerSystem::CameraManagerSystem()
{

}

wv::CameraManagerSystem::~CameraManagerSystem()
{
	EventManager* eventManager = wv::Application::getSingleton()->getEventManager();
}

void wv::CameraManagerSystem::setActiveCamera( CameraComponent* _camera )
{
	
}

void wv::CameraManagerSystem::configure( ArchetypeConfig& _config )
{
	_config.addComponentType<PlayerInputComponent>();
	_config.addComponentType<CameraComponent>();
}

void wv::CameraManagerSystem::initialize()
{

}

void wv::CameraManagerSystem::shutdown()
{

}

void wv::CameraManagerSystem::update( /*WorldUpdateContext& _ctx*/ )
{
	/*
	if ( m_cameraComponentsChanged )
	{
		if ( m_activeCamera == nullptr && m_cameraComponents.getComponents().size() > 0 )
			m_activeCamera = m_cameraComponents.getComponents()[ 0 ];
		m_cameraComponentsChanged = false;
	}

	for ( auto& [entity, playerInput, camera] : m_entityDatas )
	{
		if ( !camera )
			continue;

		if ( playerInput )
		{
			int playerIndex = playerInput->getPlayerIndex();

			if ( _ctx.inputSystem->getMouseButtonState( 2 ) )
			{
				m_cameraMove = {};

				Vector2i motion = _ctx.inputSystem->getMouseMotion();
				m_cameraMove.x += 0.4f * (float)motion.x;
				m_cameraMove.y += 0.4f * (float)motion.y;
			}
			else
			{
				const float friction = 5.f;
				const float frictionDecay = 1 / ( 1 + ( _ctx.deltaTime * friction ) );

				m_cameraMove *= frictionDecay;
			}

			m_orbitDistance -= _ctx.inputSystem->getMouseScroll() * 120 * _ctx.deltaTime;
		}

		wv::Transformf& entityTransform = entity->getTransform();

		wv::ViewVolume* viewVolume    = camera->getViewVolume();
		wv::Transformf& viewTransform = viewVolume->getTransform();

		// check if camera is orbit camera
		if ( auto orbitCamera = tryCast<OrbitCameraComponent>( camera ) )
		{
			viewTransform.rotation += wv::Vector3f{
					-m_cameraMove.y,
					-m_cameraMove.x,
					0.0f
			};

			viewTransform.setPosition( viewTransform.rotation.eulerToDirection() * m_orbitDistance );
			viewTransform.update( &entityTransform );
		}
	}

	if ( m_activeCamera )
	{
		if ( ViewVolume* viewVolume = m_activeCamera->getViewVolume() )
		{
			viewVolume->setViewDimensions( _ctx.viewport->getSize() );

			Entity* cameraEntity = m_cameraComponents.getEntity( m_activeCamera->getID() );
			viewVolume->recalculateViewMatrix( &cameraEntity->getTransform(), false );
			viewVolume->recalculateProjMatrix( true );

			_ctx.viewport->setViewVolume( viewVolume );
		}
	}
	*/

}
