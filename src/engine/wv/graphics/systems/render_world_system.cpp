#include "render_world_system.h"

#include <wv/graphics/components/mesh_component.h>
#include <wv/entity/entity.h>
#include <wv/graphics/renderer.h>

void wv::RenderWorldSystem::initialize()
{
	
}

void wv::RenderWorldSystem::shutdown()
{
	for ( auto bucket : m_renderBuckets )
		WV_FREE( bucket );

	m_renderBuckets.clear();
	m_renderBucketMap.clear();
}

void wv::RenderWorldSystem::registerComponent( Entity* _entity, IEntityComponent* _component )
{
	WV_ASSERT( _component == nullptr );
	
	MeshComponent* meshComponent = tryCast<MeshComponent>( _component );
	if ( meshComponent == nullptr ) return;

	WV_ASSERT( meshComponent->getRenderMesh().isValid() == false );

	for ( auto registeredComponent : m_registeredMeshComponents )
		if ( meshComponent == registeredComponent )
			return;
	
	if ( WorldSector* sector = _entity->getParentSector() )
	{
		WorldSectorID sectorID = sector->getID();
		RenderBucket* bucket = m_renderBucketMap[ sectorID ];
		if ( bucket == nullptr )
		{
			bucket = WV_NEW( RenderBucket );
			m_renderBucketMap.emplace( sectorID, bucket );
			m_renderBuckets.push_back( bucket );
		}

		_entity->getTransform().update( nullptr );

		bucket->entities.push_back( _entity );
		bucket->meshes.push_back( meshComponent->getRenderMesh() );
		bucket->modelMatrices.push_back( _entity->getTransform().getMatrix() );
	}

	m_registeredMeshComponents.push_back( meshComponent );
}

void wv::RenderWorldSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	MeshComponent* meshComponent = tryCast<MeshComponent>( _component );
	if ( meshComponent == nullptr ) return;

	for ( size_t i = 0; i < m_registeredMeshComponents.size(); i++ )
	{
		if ( meshComponent != m_registeredMeshComponents[ i ] )
			continue;

		m_registeredMeshComponents.erase( m_registeredMeshComponents.begin() + i );
		break;
	}

	// Remove mesh resource from bucket

	WorldSectorID sectorID = _entity->getParentSector()->getID();
	if ( RenderBucket* bucket = m_renderBucketMap.at( sectorID ) )
	{
		for ( size_t i = 0; i < bucket->meshes.size(); i++ )
		{
			if ( bucket->meshes[ i ] != meshComponent->getRenderMesh() ) 
				continue;

			bucket->meshes.erase( bucket->meshes.begin() + i );
			bucket->modelMatrices.erase( bucket->modelMatrices.begin() + i );
			bucket->entities.erase( bucket->entities.begin() + i );
			break;
		}
	}
}

void wv::RenderWorldSystem::update( WorldUpdateContext& _ctx )
{
	for ( RenderBucket* bucket : m_renderBuckets )
		for ( size_t i = 0; i < bucket->modelMatrices.size(); i++ )
			bucket->modelMatrices[ i ] = bucket->entities[ i ]->getTransform().getMatrix();
}
