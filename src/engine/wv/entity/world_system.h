#pragma once

#include <wv/reflection/reflection.h>

namespace wv {

class Entity;
class IEntityComponent;
struct WorldUpdateContext;

class IWorldSystem : IReflectedType
{
	friend class World;

	WV_REFLECT_TYPE( IWorldSystem, IReflectedType )
public:
	IWorldSystem() = default;
	virtual ~IWorldSystem() { };
	
protected:
	virtual void initialize() { }
	virtual void shutdown() { }

	virtual void registerComponent  ( Entity* _entity, IEntityComponent* _component ) = 0;
	virtual void unregisterComponent( Entity* _entity, IEntityComponent* _component ) = 0;

	virtual void update( WorldUpdateContext& _ctx ) = 0;

};

}
