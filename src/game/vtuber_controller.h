#pragma once

#include <wv/entity/entity_system.h>

class VtuberControllerSystem : public wv::IEntitySystem
{
	WV_REFLECT_TYPE( VtuberControllerSystem, wv::IEntitySystem )
public:
	VtuberControllerSystem() = default;
	~VtuberControllerSystem() { }

protected:
	virtual void initialize() override;
	virtual void shutdown() override;

	virtual void registerComponent( wv::IEntityComponent* _component ) override { };
	virtual void unregisterComponent( wv::IEntityComponent* _component ) override { };

	virtual void update( wv::WorldUpdateContext& _ctx ) override;

};