#include "mesh_render_system.h"

void wv::MeshRenderSystem::configure( ArchetypeConfig& _config )
{
	_config.addComponentType<MeshComponent>();
}

void wv::MeshRenderSystem::initialize()
{
	
}

void wv::MeshRenderSystem::shutdown()
{
	m_renderMeshes.clear();
	m_matrices.clear();
}

void wv::MeshRenderSystem::update()
{
	m_renderMeshes.clear();
	m_matrices.clear();

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& meshes = archetype->getComponents<MeshComponent>();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			wv::MeshComponent& meshComponent = meshes[ i ];

			const auto  meshAsset  = meshComponent.meshAsset;
			const auto& primitives = meshAsset->getPrimitives();

			for ( auto& primitive : meshAsset->getPrimitives() )
			{
				RenderMesh mesh{};
				mesh.mesh = meshAsset->getGPUAllocation();

				const MaterialInstance& material = meshComponent.materials[ primitive.material ];
				mesh.pipeline = material.material->getPipeline();
				mesh.materialData = {
					material.buffer.data(),
					material.buffer.size()
				};

				mesh.indexCount = primitive.indexCount;
				mesh.firstIndex = primitive.firstIndex;
				mesh.vertexOffset = primitive.vertexOffset;

				m_renderMeshes.push_back( mesh );
				
				// TODO: big buffer of matrices that updates 
				// all at once instead of PushConstants
				m_matrices.push_back( archetype->m_entities[ i ]->getTransform().getMatrix() );
			}
		}
	}
}
