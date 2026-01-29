#include "render_world_system.h"

#include <wv/rendering/components/mesh_component.h>
#include <wv/entity/entity.h>
#include <wv/entity/world.h>

void wv::RenderWorldSystem::initialize()
{
	
}

void wv::RenderWorldSystem::shutdown()
{
	m_renderBucketMap.clear();
}

void wv::RenderWorldSystem::registerComponent( Entity* _entity, IEntityComponent* _component )
{
	WV_ASSERT( _component == nullptr );
	
	MeshComponent* meshComponent = tryCast<MeshComponent>( _component );
	if ( meshComponent == nullptr ) return;

	m_meshComponents.registerComponent( _entity, _component );
	
	if ( WorldSector* sector = _entity->getParentSector() )
	{
		WorldSectorID sectorID = sector->getID();
		SectorRenderBucket& bucket = m_renderBucketMap[ sectorID ];
		
		_entity->getTransform().update( nullptr );

		MeshAsset* meshAsset = meshComponent->getMeshAsset();
		const auto& primitives = meshAsset->getPrimitives();
		const auto& materials  = meshComponent->getMaterials();

		for ( size_t i = 0; i < primitives.size(); i++ )
		{
			auto& primitive = primitives[ i ];
			RenderMesh mesh{};
			mesh.mesh = meshAsset->getGPUAllocation();

			const MaterialInstance& material = materials[ primitive.material ];
			mesh.pipeline = material.material->getPipeline();
			mesh.materialData = {
				material.buffer.data(),
				material.buffer.size()
			};
			
			mesh.indexCount = primitive.indexCount;
			mesh.firstIndex = primitive.firstIndex;
			mesh.component  = meshComponent;
			mesh.entity     = _entity;
			
			bucket.renderMeshes.push_back( mesh );
			bucket.matrices.push_back( _entity->getTransform().getMatrix() );
		}
	}
}

void wv::RenderWorldSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	MeshComponent* meshComponent = tryCast<MeshComponent>( _component );
	if ( meshComponent == nullptr ) return;

	m_meshComponents.unregisterComponent( _entity, _component );

	// Remove mesh resource from bucket
	
	WorldSectorID sectorID = _entity->getParentSector()->getID();
	auto& bucket = m_renderBucketMap.at( sectorID );
	for ( size_t i = 0; i < bucket.renderMeshes.size(); i++ )
	{
		if ( bucket.renderMeshes[ i ].component != meshComponent ) 
			continue;
		
		bucket.renderMeshes.erase( bucket.renderMeshes.begin() + i );
		bucket.matrices.erase( bucket.matrices.begin() + i );
		break;
	}
}

void wv::RenderWorldSystem::update( WorldUpdateContext& _ctx )
{
	for ( auto& pair : m_renderBucketMap )
	{
		SectorRenderBucket& bucket = pair.second;

		for ( size_t i = 0; i < bucket.matrices.size(); i++ )
		{
			auto matrix = bucket.renderMeshes[ i ].entity->getTransform().getMatrix();
			bucket.matrices[ i ] = matrix;
		}
	}
}
