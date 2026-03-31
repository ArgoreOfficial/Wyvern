#pragma once

#include <wv/entity/ecs.h>

#include <set>

namespace wv {

class EditorInterfaceSystem : public ISystem
{
//	WV_REFLECT_TYPE( EditorInterfaceSystem, IWorldSystem )
public:

	virtual void configure( ArchetypeConfig& _config ) override { }
	
};

}