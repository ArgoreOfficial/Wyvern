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
	IWorldSystem() = default;
	virtual ~IWorldSystem() { };
	
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
	virtual void initialize() override { printf( "TestWorldSystem : initialize\n" ); }
	virtual void shutdown() override { printf( "TestWorldSystem : shutdown\n" ); }

	void registerComponent  ( Entity* _entity, IEntityComponent* _component ) override { printf( "TestWorldSystem : component Registered\n" ); }
	void unregisterComponent( Entity* _entity, IEntityComponent* _component ) override { printf( "TestWorldSystem : component Registered\n" ); }

	void update( double _deltaTime ) override { 
		// printf( "TestWorldSystem updated with dt %f\n", _deltaTime );
	}

};

}