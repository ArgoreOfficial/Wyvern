#include "camera_manager_system.h"

#include <wv/camera/view_volume.h>
#include <wv/camera/components/camera_component.h>
#include <wv/camera/components/orbit_camera_component.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/graphics/viewport.h>

#include <wv/debug/log.h>
#include <wv/memory/memory.h>

wv::CameraManagerSystem::~CameraManagerSystem()
{
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

		m_cameraEntityMap.insert( { camera->getID(), _entity } );
		m_cameraComponents.push_back( camera );
		m_cameraComponentsChanged = true;
	}
}

void wv::CameraManagerSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	if ( !m_cameraEntityMap.contains( _component->getID() ) )
	{
		WV_LOG_ERROR( "CameraManagerSystem has not registered component %llu\n", _component->getID() );
		return;
	}

	if ( auto camera = tryCast<CameraComponent>( _component ) )
	{
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

	if ( m_activeCamera )
	{
		Entity* entity = m_cameraEntityMap.at( m_activeCamera->getID() );
		ViewVolume* underlyingCamera = m_activeCamera->getUnderlyingCamera();
		
		// TODO: remove transform from camera
		underlyingCamera->getTransform() = entity->getTransform();
		underlyingCamera->setPixelSize( _ctx.viewport->getSize() );
		underlyingCamera->update( _ctx.deltaTime );

		_ctx.viewport->setCamera( underlyingCamera );

	}
}
