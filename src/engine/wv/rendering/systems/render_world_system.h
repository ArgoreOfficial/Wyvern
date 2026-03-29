#pragma once

#include <wv/entity/ecs.h>
#include <wv/types.h>

#include <span>

namespace wv {

struct MeshComponent;

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

class RenderWorldSystem : public ISystem
{
//	WV_REFLECT_TYPE( RenderWorldSystem, IWorldSystem )
public:
	std::vector<SectorRenderBucket> getRenderBuckets() const {
		std::vector<SectorRenderBucket> buckets;

		for ( auto bucket : m_renderBucketMap )
			buckets.push_back( bucket.second );

		return buckets; 
	}

protected:
	virtual void configure( ArchetypeConfig& _config ) override;
	virtual void initialize() override;
	virtual void shutdown  () override;
	
	void update() override;

	std::unordered_map<UUID, SectorRenderBucket> m_renderBucketMap;
};

}