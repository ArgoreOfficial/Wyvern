#pragma once

#include <wv/reflection/reflection.h>

namespace wv {

class IEntityComponent;

class IEntitySystem : IReflectedType
{
	friend class Entity;

	WV_REFLECT_TYPE( IEntitySystem )
public:
	
protected:
	virtual void initialize() { }
	virtual void shutdown() { }

	virtual void registerComponent  ( IEntityComponent* _component ) = 0;
	virtual void unregisterComponent( IEntityComponent* _component ) = 0;

	virtual void update( double _deltaTime ) = 0;

};

class TestEntitySystem : public IEntitySystem
{
	WV_REFLECT_TYPE( TestEntitySystem )
public:
	TestEntitySystem() = default;

protected:
	virtual void initialize() override { printf( "TestEntitySystem : initialize\n" ); }
	virtual void shutdown() override { printf( "TestEntitySystem : shutdown\n" ); }

	void registerComponent( IEntityComponent* _component ) override { printf( "TestEntitySystem : component Registered\n" ); }
	void unregisterComponent( IEntityComponent* _component ) override { printf( "TestEntitySystem : component Registered\n" ); }

	void update( double _deltaTime ) override { 
		// printf( "TestEntitySystem updated with dt %f\n", _deltaTime );
	}

};

}