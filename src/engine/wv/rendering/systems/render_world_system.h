#pragma once

#include <wv/entity/entity_component.h>
#include <wv/entity/entity_component_container.h>
#include <wv/entity/world_system.h>
#include <wv/entity/world_sector.h>
#include <wv/types.h>

#include <span>

namespace wv {

class MeshComponent;

struct RenderMesh
{
	ResourceID mesh = {};
	ResourceID pipeline = {};
	uint32_t firstIndex = 0;
	uint32_t indexCount = 0;
	uint32_t vertexOffset = 0;
	std::span<const uint8_t> materialData = {};

	Entity* entity = nullptr;
	MeshComponent* component = nullptr;
};

struct SectorRenderBucket
{
	std::vector<RenderMesh> renderMeshes;
	std::vector<Matrix4x4f> matrices;
};

class RenderWorldSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( RenderWorldSystem, IWorldSystem )
public:
	RenderWorldSystem() = default;

	const std::vector<MeshComponent*>& getRegisteredMeshComponents() const { return m_meshComponents.getComponents(); }

	std::vector<SectorRenderBucket> getRenderBuckets() const {
		std::vector<SectorRenderBucket> buckets;

		for ( auto bucket : m_renderBucketMap )
			buckets.push_back( bucket.second );

		return buckets; 
	}

protected:
	virtual void initialize() override;
	virtual void shutdown  () override;
	
	virtual void registerComponent  ( Entity* _entity, IEntityComponent* _component ) override;
	virtual void unregisterComponent( Entity* _entity, IEntityComponent* _component ) override;

	void update( WorldUpdateContext& _ctx ) override;

	EntityComponentContainer<MeshComponent> m_meshComponents;
	std::unordered_map<WorldSectorID, SectorRenderBucket> m_renderBucketMap;
};

}