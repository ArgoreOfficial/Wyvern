#include "render_world_system.h"

#include <wv/rendering/components/mesh_component.h>

void wv::RenderWorldSystem::configure( ArchetypeConfig& _config )
{
	_config.addComponentType<MeshComponent>();
}

void wv::RenderWorldSystem::initialize()
{
	
}

void wv::RenderWorldSystem::shutdown()
{
	m_renderBucketMap.clear();
}

void wv::RenderWorldSystem::update()
{
	/*
	for ( auto& pair : m_renderBucketMap )
	{
		SectorRenderBucket& bucket = pair.second;

		for ( size_t i = 0; i < bucket.matrices.size(); i++ )
		{
			auto matrix = bucket.renderMeshes[ i ].entity->getTransform().getMatrix();
			bucket.matrices[ i ] = matrix;
		}
	}
	*/
}
