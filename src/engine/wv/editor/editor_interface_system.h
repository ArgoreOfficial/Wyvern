#pragma once

#include <wv/entity/world_system.h>
#include <wv/entity/entity_component.h>
#include <wv/entity/entity_component_container.h>

#include <set>

namespace wv {

struct SceneViewSelection
{
	wv::Entity* selectedEntity = nullptr;
	wv::IEntityComponent* selectedComponent = nullptr;
	wv::IWorldSystem* selectedWorldSystem = nullptr;
};

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

	virtual void update( WorldUpdateContext& _ctx ) override;
	virtual void onDebugRender() override;

	void renderPrimaryMenuBar();
	void renderSecondaryMenuBar();
	void renderStatusBar();

	void renderWorldWindow();

	EntityComponentContainer<IEntityComponent> m_components;
	SceneViewSelection m_selection{};

	double m_timeSinceFPSUpdate = 0.0;
	size_t m_framesSinceFPSUpdate = 0;
	double m_averageFPS = 0.0;

	bool m_showRenderWorldWindow = false;
};

}