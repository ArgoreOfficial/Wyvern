#include "camera_manager_system.h"

#include <wv/application.h>
#include <wv/camera/view_volume.h>
#include <wv/camera/components/camera_component.h>
#include <wv/camera/components/orbit_camera_component.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/graphics/viewport.h>

#include <wv/debug/log.h>
#include <wv/memory/memory.h>
#include <wv/input/input_system.h>
#include <wv/event/event_manager.h>

wv::CameraManagerSystem::CameraManagerSystem()
{

}

wv::CameraManagerSystem::~CameraManagerSystem()
{
	EventManager* eventManager = wv::Application::getSingleton()->getEventManager();

	m_cameraComponents.clear();
}

void wv::CameraManagerSystem::setActiveCamera( CameraComponent* _camera )
{
	for ( CameraComponent* comp : m_cameraComponents )
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
		m_jumpAction = playerActions->getTriggerActionID( "Jump" );
		m_lookAction = playerActions->getAxisActionID( "Look" );
	}
}

void wv::CameraManagerSystem::shutdown()
{
	
}

void wv::CameraManagerSystem::registerComponent( Entity* _entity, IEntityComponent* _component )
{
	CameraComponent* camera = tryCast<CameraComponent>( _component );
	if ( camera == 0 ) 
		camera = tryCast<OrbitCameraComponent>( _component ); // TODO: derivation chain

	if ( camera )
	{
		for ( auto registeredComponent : m_cameraComponents )
			if ( camera == registeredComponent ) return;

		camera->getViewVolume()->recalculateProjMatrix( false );

		m_cameraEntityMap.insert( { camera->getID(), _entity } );
		m_cameraComponents.push_back( camera );
		m_cameraComponentsChanged = true;
	}
}

void wv::CameraManagerSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	if ( auto camera = tryCast<CameraComponent>( _component ) )
	{
		if ( !m_cameraEntityMap.contains( camera->getID() ) )
		{
			WV_LOG_ERROR( "CameraManagerSystem has not registered component %llu\n", _component->getID() );
			return;
		}

		for ( size_t i = 0; i < m_cameraComponents.size(); i++ )
		{
			if ( camera != m_cameraComponents[ i ] ) continue;

			m_cameraEntityMap.erase( _component->getID() );
			m_cameraComponents.erase( m_cameraComponents.begin() + i );
			m_cameraComponentsChanged = true;
			break;
		}
	}
}

void wv::CameraManagerSystem::update( WorldUpdateContext& _ctx )
{
	if ( m_cameraComponentsChanged )
	{
		if ( m_activeCamera == nullptr && m_cameraComponents.size() > 0 )
			m_activeCamera = m_cameraComponents[ 0 ];
		m_cameraComponentsChanged = false;
	}

	Vector2f cameraMove = { 0.0f, 0.0f };

	for ( const ActionEvent& event : _ctx.actionEventQueue )
	{
		if ( event.actionID == m_jumpAction && event.action.trigger->state )
			wv::Debug::Print( "Jumped!\n" );
		if ( event.actionID == m_lookAction )
			cameraMove = event.action.axis->value * 90.0f * _ctx.deltaTime;
	}

	if ( ActionGroup* playerActions = _ctx.inputSystem->getActionGroup( "Player" ) )
	{
		wv::Vector2f move = playerActions->getAxisValue( "Move" );
		m_orbitDistance -= move.y * _ctx.deltaTime * 10.0;
		m_orbitDistance = wv::Math::max( m_orbitDistance, 2.25f );

		cameraMove = playerActions->getAxisValue( "Look" );
	}

	if ( m_activeCamera )
	{
		Entity* entity = m_cameraEntityMap.at( m_activeCamera->getID() );
		ViewVolume* viewVolume = m_activeCamera->getViewVolume();
		
		if ( auto orbitCamera = tryCast<OrbitCameraComponent>( m_activeCamera ) )
		{
			wv::Transformf& transform = entity->getTransform();
			
			if ( _ctx.inputSystem->debugIsMouseDown( 1 ) )
				cameraMove = _ctx.inputSystem->debugGetMouseMotion() * 0.3f;
			
			transform.rotation += wv::Vector3f{
					-cameraMove.y,
					-cameraMove.x,
					0.0f
			}; 
			transform.setPosition( transform.rotation.eulerToDirection() * m_orbitDistance );
		}

		viewVolume->getTransform() = entity->getTransform();
		viewVolume->setViewDimensions( _ctx.viewport->getSize() );
		viewVolume->update( _ctx.deltaTime );

		_ctx.viewport->setViewVolume( viewVolume );
	}
}
