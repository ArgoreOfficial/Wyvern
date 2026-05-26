#include "camera_manager_system.h"

#include <wv/application.h>
#include <wv/components/camera_component.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/rendering/viewport.h>

#include <wv/rendering/renderer.h>

void wv::CameraManagerSystem::configure( ArchetypeConfig& _config )
{
	m_debugName = "CameraManagerSystem";

	setUpdateMode( UpdateMode_Always );
	setEditorRenderEnabled( true );

	_config.addComponentType<CameraComponent>();
}

void wv::CameraManagerSystem::onEditorRender()
{
	auto renderer = getApp()->getRenderer();
	World* world = getWorld();

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& cameras = archetype->getComponents<CameraComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			if ( !cameras[ i ].active )
				continue;

			auto& cam = cameras[ i ];
			
			cam.viewDimensions = world->getViewport()->size;

			cam.recalculateViewMatrix( &entities[ i ]->getTransform(), false);
			cam.recalculateProjMatrix( true );

			{
				const Vector3f tlNear = cam.screenToWorld( -1.0f,  1.0f, 0.25f );
				const Vector3f trNear = cam.screenToWorld(  1.0f,  1.0f, 0.25f );
				const Vector3f blNear = cam.screenToWorld( -1.0f, -1.0f, 0.25f );
				const Vector3f brNear = cam.screenToWorld(  1.0f, -1.0f, 0.25f );
			
				const Vector3f tlFar = cam.screenToWorld( -1.0f,  1.0f, 5.0f );
				const Vector3f trFar = cam.screenToWorld(  1.0f,  1.0f, 5.0f );
				const Vector3f blFar = cam.screenToWorld( -1.0f, -1.0f, 5.0f );
				const Vector3f brFar = cam.screenToWorld(  1.0f, -1.0f, 5.0f );
			
				renderer->addDebugLine( tlNear, trNear );
				renderer->addDebugLine( trNear, brNear );
				renderer->addDebugLine( brNear, blNear );
				renderer->addDebugLine( blNear, tlNear );
			
				renderer->addDebugLine( tlFar, trFar );
				renderer->addDebugLine( trFar, brFar );
				renderer->addDebugLine( brFar, blFar );
				renderer->addDebugLine( blFar, tlFar );
				
				renderer->addDebugLine( tlNear, tlFar );
				renderer->addDebugLine( trNear, trFar );
				renderer->addDebugLine( brNear, brFar );
				renderer->addDebugLine( blNear, blFar );
			}
		}
	}
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
			//WV_LOG_ERROR( "No Camera\n" );
			return;
		}

		CameraComponent& camera = getArchetypes()[ 0 ]->getComponents<CameraComponent>()[ 0 ];
		cameraToUpdate = getArchetypes()[ 0 ]->getEntities()[ 0 ];
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
	world->getViewport()->viewPos = _entity->getTransform().position;
	
}
