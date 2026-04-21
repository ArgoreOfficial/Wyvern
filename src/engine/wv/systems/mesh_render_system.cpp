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
	World* world = wv::getApp()->getWorld();
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
	World* world = wv::getApp()->getWorld();

	for ( Archetype* archetype : getArchetypes() )
	{
		auto& meshes   = archetype->getComponents<MeshComponent>();
		auto& entities = archetype->getEntities();

		for ( size_t i = 0; i < archetype->getNumEntities(); i++ )
		{
			wv::MeshComponent& meshComponent = meshes[ i ];

			if ( meshComponent.meshAsset == nullptr )
			{
				if ( !meshComponent.assetPath.empty() && meshComponent.loadState == meshComponent.LoadState_unloaded )
				{
					meshComponent.loadState = MeshComponent::LoadState_loading;

					MeshImporterGLTF importer = wv::MeshImporterGLTF( app->getMeshManager(), app->getMaterialManager(), world->getTextureManager() );
					importer.load( meshComponent.assetPath, meshComponent.importOptions );

					if ( importer.hasLoaded() )
					{
						meshComponent.materials = importer.getMaterials();
						meshComponent.meshAsset = importer.getMesh();
						meshComponent.loadState = MeshComponent::LoadState_loaded;
					}
					else
					{
						meshComponent.loadState = MeshComponent::LoadState_loadFailed;
					}
				}

				continue;
			}

			const auto  meshAsset  = meshComponent.meshAsset;
			const auto& primitives = meshAsset->getPrimitives();

			for ( auto& primitive : meshAsset->getPrimitives() )
			{
				/*
				RenderMesh mesh{};
				mesh.mesh = meshAsset->getGPUAllocation();
				if ( primitive.material < meshComponent.materials.size() )
				{
					Ref<MaterialAsset> material = meshComponent.materials[ primitive.material ];
					mesh.pipeline = material->shaderType->getPipelineID();
					mesh.materialData = {
						material.buffer.data(),
						material.buffer.size()
					};
				}
				else
				{
					// no material assigned, use default
					mesh.pipeline = m_defaultMaterialInstance.material->getPipeline();
					mesh.materialData = {
						m_defaultMaterialInstance.buffer.data(),
						m_defaultMaterialInstance.buffer.size()
					};
				}

				mesh.indexCount = primitive.indexCount;
				mesh.firstIndex = primitive.firstIndex;
				mesh.vertexOffset = primitive.vertexOffset;

				m_renderMeshes.push_back( mesh );
				
				// TODO: big buffer of matrices that updates 
				// all at once instead of PushConstants
				m_matrices.push_back( entities[ i ]->getTransform().getMatrix());
				*/
			}
		}
	}
}
