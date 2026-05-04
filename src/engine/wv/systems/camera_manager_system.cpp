#include "camera_manager_system.h"

#include <wv/application.h>
#include <wv/components/camera_component.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/rendering/viewport.h>

void wv::CameraManagerSystem::configure( ArchetypeConfig& _config )
{
	m_debugName = "CameraManagerSystem";

	setUpdateMode( UpdateMode_Always );

	_config.addComponentType<CameraComponent>();
}

void wv::CameraManagerSystem::onInternalCameraUpdate()
{
	Entity* cameraToUpdate = nullptr;
	int numActiveCameras = 0;

	m_activeCameras.clear();

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& cameras  = archetype->getComponents<CameraComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			if ( !cameras[ i ].active )
				continue;
			
			m_activeCameras.push_back( entities[ i ] );
		}
	}
	
	if ( !m_activeCameras.empty() )
		cameraToUpdate = m_activeCameras[ 0 ];
	else
	{
		// no active cameras

		if ( getArchetypes().empty() )
		{
			WV_LOG_ERROR( "No Camera\n" );
			return;
		}

		CameraComponent& camera = getArchetypes()[ 0 ]->getComponents<CameraComponent>()[ 0 ];
		cameraToUpdate = getArchetypes()[ 0 ]->getEntities()[ 0 ];

		camera.active = true;
	}

	if ( m_cameraOverride )
		cameraToUpdate = m_cameraOverride;
	
	if( cameraToUpdate )
	{
		size_t idx = cameraToUpdate->archetype->getEntityIndex( cameraToUpdate );
		CameraComponent& camera = cameraToUpdate->archetype->getComponents<CameraComponent>()[ idx ];
		
		updateCamera( cameraToUpdate, camera );
	}
}

void wv::CameraManagerSystem::updateCamera( Entity* _entity, CameraComponent& _component )
{
	World* world = getWorld();

	_component.viewDimensions = world->getViewport()->size;

	_component.recalculateViewMatrix( &_entity->getTransform(), false );
	_component.recalculateProjMatrix( true );

	world->getViewport()->viewProj = _component.viewProjMatrix;
	
}
