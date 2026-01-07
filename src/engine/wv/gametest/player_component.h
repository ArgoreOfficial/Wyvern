#pragma once

#include <wv/entity/entity_component.h>

namespace wv {

class PlayerControllerComponent : public IEntityComponent
{
	friend class PlayerControllerComponent;

	WV_REFLECT_TYPE( PlayerControllerComponent )
public:
	PlayerControllerComponent() = default;
	virtual ~PlayerControllerComponent() { }

protected:

};

}