#include "camera_manager_system.h"

#include <wv/application.h>
#include <wv/components/camera_component.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/rendering/viewport.h>

void wv::CameraManagerSystem::configure( ArchetypeConfig& _config )
{
	_config.addComponentType<CameraComponent>();
}

void wv::CameraManagerSystem::onInternalCameraUpdate()
{
	int numActiveCameras = 0;

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& cameras  = archetype->getComponents<CameraComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			if ( !cameras[ i ].active )
				continue;

			numActiveCameras++;

			// TODO: allow multi-camera rendering
			if ( numActiveCameras > 1 ) // only update first active camera
				continue;
			
			updateCamera( entities[ i ], cameras[ i ] );
		}
	}

	if ( numActiveCameras == 0 )
	{
		if ( getArchetypes().empty() )
		{
			WV_LOG_ERROR( "No Camera\n" );
			return;
		}

		CameraComponent& camera = getArchetypes()[ 0 ]->getComponents<CameraComponent>()[ 0 ];
		Entity*          entity = getArchetypes()[ 0 ]->getEntities()[ 0 ];
		
		camera.active = true;

		updateCamera( entity, camera );
	}
}

void wv::CameraManagerSystem::updateCamera( Entity* _entity, CameraComponent& _component )
{
	World* world = wv::getApp()->getWorld();

	_component.viewDimensions = world->getViewport()->size;

	_component.recalculateViewMatrix( &_entity->getTransform(), false );
	_component.recalculateProjMatrix( true );

	world->getViewport()->viewProj = _component.viewProjMatrix;
	
}
