#include "camera_manager_system.h"

#include <wv/camera/camera.h>
#include <wv/camera/components/camera_component.h>
#include <wv/camera/components/orbit_camera_component.h>

#include <wv/debug/log.h>
#include <wv/memory/memory.h>

wv::CameraManagerSystem::~CameraManagerSystem()
{
	if ( m_cameraComponents.size() > 0 )
	{
		WV_LOG_WARNING( "CameraManagerSystem contains %zu camera components that will be deleted\n", m_cameraComponents.size() );
	}

	for ( auto camera : m_cameraComponents )
		WV_FREE( camera );
	
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

		m_cameraComponents.push_back( camera );
		m_cameraComponentsChanged = true;
	}
}

void wv::CameraManagerSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	if ( auto camera = tryCast<CameraComponent>( _component ) )
	{
		for ( size_t i = 0; i < m_cameraComponents.size(); i++ )
		{
			if ( camera != m_cameraComponents[ i ] ) continue;

			m_cameraComponents.erase( m_cameraComponents.begin() + i );
			m_cameraComponentsChanged = true;
			break;
		}
	}
}

void wv::CameraManagerSystem::update( double _deltaTime )
{
	if ( m_cameraComponentsChanged )
	{
		if ( m_activeCamera == nullptr && m_cameraComponents.size() > 0 )
			m_activeCamera = m_cameraComponents[ 0 ];
		m_cameraComponentsChanged = false;
	}

	if( hasActiveCamera() )
		m_activeCamera->getUnderlyingCamera()->update( _deltaTime );
}
