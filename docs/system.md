```cpp
#pragma once
#include <wv/entity/entity_system.h>

class ExampleEntitySystem : public wv::IEntitySystem
{
	WV_REFLECT_TYPE( ExampleEntitySystem )
public:
	ExampleEntitySystem() = default;

protected:
	virtual void initialize() override;
	virtual void shutdown  () override;

	void registerComponent  ( wv::IEntityComponent* _component ) override;
	void unregisterComponent( wv::IEntityComponent* _component ) override;

	void update( double _deltaTime ) override;

};
```