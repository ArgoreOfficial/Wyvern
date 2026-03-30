#pragma once

#include <wv/entity/ecs.h>
#include <wv/types.h>

#include <wv/rendering/components/mesh_component.h>

#include <span>

namespace wv {

struct MeshComponent;


class RenderWorldSystem : public ISystem
{
public:
	std::vector<RenderMesh>& getRenderMeshes() { return m_renderMeshes; }
	std::vector<Matrix4x4f>& getMatrices()     { return m_matrices; }

protected:
	virtual void configure( ArchetypeConfig& _config ) override;
	virtual void initialize() override;
	virtual void shutdown  () override;
	
	void update() override;

	std::vector<RenderMesh> m_renderMeshes;
	std::vector<Matrix4x4f> m_matrices;
};

}