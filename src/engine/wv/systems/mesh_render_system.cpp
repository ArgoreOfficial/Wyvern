#include "mesh_render_system.h"

#include <wv/application.h>
#include <wv/entity/world.h>
#include <wv/editor/mesh_importer_gltf.h>

#include <wv/rendering/texture.h>

void wv::MeshRenderSystem::configure( ArchetypeConfig& _config )
{
	m_debugName = "MeshRenderSystem";

	setUpdateMode( UpdateMode_always );

	_config.addComponentType<MeshComponent>();
}

void wv::MeshRenderSystem::onInitialize()
{
	m_defaultMaterial = wv::getApp()->getMaterialManager()->get( "materials/default_lit.wvmt" );
}

void wv::MeshRenderSystem::onShutdown()
{
	m_renderMeshes.clear();
	m_matrices.clear();
}

void wv::MeshRenderSystem::onRender()
{
	m_renderMeshes.clear();
	m_matrices.clear();

	Application* app = wv::getApp();
	
	for ( Archetype* archetype : getArchetypes() )
	{
		auto& meshes   = archetype->getComponents<MeshComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			wv::MeshComponent& meshComponent = meshes[ i ];

			if ( meshComponent.meshAsset == nullptr )
				continue;
			
			const auto  meshAsset  = meshComponent.meshAsset;
			const auto& primitives = meshAsset->getPrimitives();

			for ( auto& primitive : meshAsset->getPrimitives() )
			{
				RenderMesh mesh{};
				mesh.mesh = meshAsset->getGPUAllocation();
				
				Ref<MaterialAsset> material = nullptr;

				if ( primitive.material < meshComponent.materials.size() )
					material = meshComponent.materials[ primitive.material ];
				
				if( !material || !material->shaderType )
					material = m_defaultMaterial;

				mesh.pipeline = material->shaderType->getPipelineID();
				mesh.materialBuffer = material->shaderType->getBufferID();
				mesh.materialIndex = material->m_materialIndex;


				mesh.indexCount = primitive.indexCount;
				mesh.firstIndex = primitive.firstIndex;
				mesh.vertexOffset = primitive.vertexOffset;

				m_renderMeshes.push_back( mesh );
				
				// TODO: big buffer of matrices that updates 
				// all at once instead of PushConstants
				m_matrices.push_back( entities[ i ]->getTransform().getMatrix());
			}
		}
	}
}
