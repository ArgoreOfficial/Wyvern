#pragma once

#include <wv/reflection/reflection.h>

namespace wv {

class Entity;
class IEntityComponent;

class IWorldSystem : IReflectedType
{
	friend class World;

	WV_REFLECT_TYPE( IWorldSystem )
public:
	
protected:
	virtual void initialize() { }
	virtual void shutdown() { }

	virtual void registerComponent  ( Entity* _entity, IEntityComponent* _component ) = 0;
	virtual void unregisterComponent( Entity* _entity, IEntityComponent* _component ) = 0;

	virtual void update( double _deltaTime ) = 0;

};

class TestWorldSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( TestWorldSystem )
public:
	TestWorldSystem() = default;

protected:
	void registerComponent  ( Entity* _entity, IEntityComponent* _component ) override { }
	void unregisterComponent( Entity* _entity, IEntityComponent* _component ) override { }

	void update( double _deltaTime ) override { 
		printf( "TestWorldSystem updated with dt %f\n", _deltaTime );
	}

};

}