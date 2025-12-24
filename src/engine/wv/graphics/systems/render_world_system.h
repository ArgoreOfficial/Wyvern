#pragma once

#include <wv/entity/world_system.h>
#include <wv/entity/world_sector.h>

#include <unordered_map>

namespace wv {

class MeshComponent;
struct WorldUpdateContext;
struct RenderBucket;

class RenderWorldSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( RenderWorldSystem )
public:
	RenderWorldSystem() = default;

	const std::vector<MeshComponent*>& getRegisteredMeshComponents() const { return m_registeredMeshComponents; }
	const std::vector<RenderBucket*>&  getRenderBuckets()            const { return m_renderBuckets; }

protected:
	virtual void initialize() override;
	virtual void shutdown  () override;
	
	virtual void registerComponent  ( Entity* _entity, IEntityComponent* _component ) override;
	virtual void unregisterComponent( Entity* _entity, IEntityComponent* _component ) override;

	void update( WorldUpdateContext& _ctx ) override;

	std::vector<MeshComponent*> m_registeredMeshComponents;
	std::vector<RenderBucket*> m_renderBuckets;
	std::unordered_map<WorldSectorID, RenderBucket*> m_renderBucketMap;
};

}