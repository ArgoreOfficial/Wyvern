#pragma once

#include <wv/reflection/reflection.h>

namespace wv {

class IEntityComponent;

class IEntitySystem : IReflectedType
{
	friend class Entity;

	WV_REFLECT_TYPE( IEntitySystem )
public:
	IEntitySystem() = default;
	virtual ~IEntitySystem() { };

protected:
	virtual void initialize() { }
	virtual void shutdown() { }

	virtual void registerComponent  ( IEntityComponent* _component ) = 0;
	virtual void unregisterComponent( IEntityComponent* _component ) = 0;

	virtual void update( double _deltaTime ) = 0;

};

}