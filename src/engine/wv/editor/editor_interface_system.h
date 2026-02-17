#pragma once

#include <wv/entity/world_system.h>
#include <wv/entity/entity_component.h>
#include <wv/entity/entity_component_container.h>

#include <set>

namespace wv {

class EditorInterfaceSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( EditorInterfaceSystem, IWorldSystem )
public:
	 EditorInterfaceSystem() { }
	~EditorInterfaceSystem() { }

protected:
	virtual void initialize() override { }
	virtual void shutdown() override { }

	virtual void registerComponent( Entity* _entity, IEntityComponent* _component ) override { 
		m_components.registerComponent( _entity, _component );
	}
	virtual void unregisterComponent( Entity* _entity, IEntityComponent* _component ) override { 
		m_components.unregisterComponent( _entity, _component );
	}

	virtual void update( WorldUpdateContext& _ctx ) override { }
	virtual void onDebugRender() override;

	EntityComponentContainer<IEntityComponent> m_components;

	//std::set<UUID> m_selectedEntities;
	UUID m_selectedUUID{ 0 };
};

}