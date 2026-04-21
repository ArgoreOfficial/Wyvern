#pragma once

#include <wv/components/mesh_component.h>

#include <wv/entity/ecs.h>

namespace wv {

class MeshRenderSystem : public ISystem
{
public:
	std::vector<RenderMesh>& getRenderMeshes() { return m_renderMeshes; }
	std::vector<Matrix4x4f>& getMatrices()     { return m_matrices; }

protected:
	virtual void configure( ArchetypeConfig& _config ) override;

	virtual void onInitialize() override;
	virtual void onShutdown() override;
	virtual void onRender() override;
	
	Ref<MaterialAsset> m_defaultMaterial;

	wv::Ref<TextureAsset> m_prototypeTexture;

	std::vector<RenderMesh> m_renderMeshes;
	std::vector<Matrix4x4f> m_matrices;
};

}