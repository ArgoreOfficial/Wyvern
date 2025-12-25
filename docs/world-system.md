

## Example WorldSystem header
```cpp
#pragma once
#include <wv/entity/world_system.h>

class ExampleWorldSystem : public wv::IWorldSystem
{
	WV_REFLECT_TYPE( ExampleWorldSystem )
public:
	ExampleWorldSystem() = default;

protected:
	virtual void initialize() override;
	virtual void shutdown  () override;
	
	virtual void registerComponent  ( wv::Entity* _entity, wv::IEntityComponent* _component ) override;
	virtual void unregisterComponent( wv::Entity* _entity, wv::IEntityComponent* _component ) override;

	void update( wv::WorldUpdateContext& _ctx ) override;
};
```