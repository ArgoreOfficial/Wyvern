#include "render_world_system.h"

#include <wv/rendering/components/mesh_component.h>
#include <wv/entity/entity.h>

#include <wv/application.h>
#include <wv/rendering/renderer.h>
#include <wv/filesystem/loaders/mesh_asset_loader.h>

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

		meshComponent->getMeshAsset();

		RenderMesh mesh;
		mesh.assetID = meshComponent->getMeshAsset();
		mesh.component = meshComponent;
		mesh.entity = _entity;

		if ( mesh.assetID.isValid() )
		{
			WV_LOG_WARNING( "Mesh creation\n" );
			Renderer* renderer = Application::getSingleton()->getRenderer();

			MeshAssetLoader* meshAssetLoader = sector->getMeshAssetLoader();
			MeshAsset* meshAsset = meshAssetLoader->getMeshAsset( mesh.assetID );

			std::vector<VertexData> datas;
			for ( size_t i = 0; i < meshAsset->vertexPositions.size(); i++ )
			{
				VertexData data;

				if( meshAsset->vertexNormals.size() > 0 ) data.normal   = meshAsset->vertexNormals[ i ];
				if( meshAsset->vertexColours.size() > 0 ) data.color    = meshAsset->vertexColours[ i ];
				if( meshAsset->vertexUVs.size() > 0 )     data.texCoord = meshAsset->vertexUVs[ i ];

				datas.push_back( data );
			}

			mesh.meshID = renderer->createMesh( meshAsset->indices, meshAsset->vertexPositions );
				
		//	if ( mesh.meshID.isValid() )
		//		renderer->setRenderMeshMaterial( mesh.meshID, meshComponent->getMaterial() );
		}

		bucket.renderMeshes.push_back( mesh );
		bucket.matrices.push_back( _entity->getTransform().getMatrix() );
	}
}

void wv::RenderWorldSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	MeshComponent* meshComponent = tryCast<MeshComponent>( _component );
	if ( meshComponent == nullptr ) return;

	m_meshComponents.unregisterComponent( _entity, _component );

	// Remove mesh resource from bucket
	
	Renderer* renderer = Application::getSingleton()->getRenderer();
	
	WorldSectorID sectorID = _entity->getParentSector()->getID();
	auto& bucket = m_renderBucketMap.at( sectorID );
	for ( size_t i = 0; i < bucket.renderMeshes.size(); i++ )
	{
		if ( bucket.renderMeshes[ i ].component != meshComponent ) 
			continue;
		
		renderer->destroyMesh( bucket.renderMeshes[ i ].meshID );

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
