#pragma once

#include <wv/reflection/reflection.h>

namespace wv {

class IEntityComponent;

struct WorldUpdateContext;

class IEntitySystem : IReflectedType
{
	friend class Entity;

	WV_REFLECT_TYPE( IEntitySystem, IReflectedType )
public:
	IEntitySystem() = default;
	virtual ~IEntitySystem() { };

protected:
	virtual void initialize() { }
	virtual void shutdown() { }

	virtual void registerComponent  ( IEntityComponent* _component ) = 0;
	virtual void unregisterComponent( IEntityComponent* _component ) = 0;

	virtual void update( WorldUpdateContext& _ctx ) = 0;

	bool    m_initialized = false;
	Entity* m_entity      = nullptr;
};

}