```cpp
#pragma once
#include <wv/entity/entity_component.h>

class ExampleComponent : public wv::IEntityComponent
{
	WV_REFLECT_TYPE( ExampleComponent )
public:
	ExampleComponent() = default;

protected:
	// virtual void load()       override;
	// virtual void unload()     override;
	// virtual void initialize() override;
	// virtual void shutdown()   override;
	
};
```