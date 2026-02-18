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
#include <wv/memory/memory.h>
#include <wv/input/input_system.h>
#include <wv/event/event_manager.h>

#include <inttypes.h>
#include <imgui/imgui.h>

wv::CameraManagerSystem::CameraManagerSystem()
{

}

wv::CameraManagerSystem::~CameraManagerSystem()
{
	EventManager* eventManager = wv::Application::getSingleton()->getEventManager();
}

void wv::CameraManagerSystem::setActiveCamera( CameraComponent* _camera )
{
	for ( CameraComponent* comp : m_cameraComponents.getComponents() )
	{
		if ( comp != _camera )
			continue;

		m_activeCamera = _camera;
		return;
	}
	
	WV_LOG_ERROR( "Camera is not registered with CameraManagerSystem\n" );
}

void wv::CameraManagerSystem::initialize()
{
	InputSystem* inputSystem = wv::Application::getSingleton()->getInputSystem();
	
	if ( ActionGroup* playerActions = inputSystem->getActionGroup( "Player" ) )
	{
		m_lookAction = playerActions->getAxisActionID( "Look" );
	}
}

void wv::CameraManagerSystem::shutdown()
{
	m_entityDatas.clear();
}

void wv::CameraManagerSystem::registerComponent( Entity* _entity, IEntityComponent* _component )
{
	if ( auto camera = tryCast<CameraComponent>( _component ) )
	{
		if ( !m_cameraComponents.registerComponent( _entity, _component ) )
			return;

		auto entityIt = findEntity( _entity );
		if ( entityIt == m_entityDatas.end() )
		{
			m_entityDatas.emplace_back( _entity );
			entityIt = findEntity( _entity );
			WV_ASSERT( entityIt != m_entityDatas.end() );
		}

		camera->getViewVolume()->recalculateProjMatrix( false );
		m_cameraComponentsChanged = true;
		
		entityIt->camera = camera;
		entityIt->entity->getTransform().addChild( &camera->getViewVolume()->getTransform() );
	}
	else if ( PlayerInputComponent* playerInput = tryCast<PlayerInputComponent>( _component ) )
	{
		if ( !m_playerInputComponents.registerComponent( _entity, _component ) )
			return;

		auto entityIt = findEntity( _entity );
		if ( entityIt == m_entityDatas.end() )
		{
			m_entityDatas.emplace_back( _entity );
			entityIt = findEntity( _entity );
			WV_ASSERT( entityIt != m_entityDatas.end() );
		}

		entityIt->playerInput = playerInput;
	}
}

void wv::CameraManagerSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	if ( auto camera = tryCast<CameraComponent>( _component ) )
	{
		if ( !m_cameraComponents.unregisterComponent( _entity, _component ) )
			return;
		
		auto entityIt = findEntity( _entity );
		if ( entityIt != m_entityDatas.end() && entityIt->camera == camera )
			entityIt->camera = nullptr;
	}
	else if ( auto playerInput = tryCast<PlayerInputComponent>( _component ) )
	{
		if ( !m_playerInputComponents.unregisterComponent( _entity, _component ) )
			return;

		auto entityIt = findEntity( _entity );
		if ( entityIt != m_entityDatas.end() && entityIt->playerInput == playerInput )
			entityIt->playerInput = nullptr;
	}
}

void wv::CameraManagerSystem::update( WorldUpdateContext& _ctx )
{
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

			for ( const ActionEvent& event : _ctx.actionEventQueue )
			{
				if ( event.playerIndex != playerIndex )
					continue;

				if ( event.actionID == m_lookAction )
					m_cameraMove = event.action.axis->getValue( playerIndex ) * 90.0f * _ctx.deltaTime;
			}

			if ( _ctx.inputSystem->getMouseButtonState( 1 ) )
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

			if ( ActionGroup* playerActions = _ctx.inputSystem->getActionGroup( "Player" ) )
			{
				wv::Vector2f move = playerActions->getAxisValue( playerIndex, "CameraZoom" );
				m_orbitDistance -= move.y * _ctx.deltaTime * 10.0;
				m_orbitDistance = wv::Math::max( m_orbitDistance, 2.25f );
			}
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

}
