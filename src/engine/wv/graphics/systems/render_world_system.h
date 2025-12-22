#pragma once

#include <wv/entity/world_system.h>

namespace wv {

class MeshComponent;

class RenderWorldSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( RenderWorldSystem )
public:
	RenderWorldSystem() = default;

	const std::vector<MeshComponent*>& getRegisteredMeshComponents() const { return m_registeredMeshComponents; }

protected:
	virtual void initialize() override;
	virtual void shutdown  () override;
	
	virtual void registerComponent  ( Entity* _entity, IEntityComponent* _component ) override;
	virtual void unregisterComponent( Entity* _entity, IEntityComponent* _component ) override;

	void update( double _deltaTime ) override;

	std::vector<MeshComponent*> m_registeredMeshComponents;
	
};

}