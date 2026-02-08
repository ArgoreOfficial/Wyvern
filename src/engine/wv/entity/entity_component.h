#pragma once

#include <wv/reflection/reflection.h>
#include <wv/math/math.h>


namespace wv {

typedef uint32_t ComponentID;

struct WorldLoadContext;

class IEntityComponent : public wv::IReflectedType
{
	friend class Entity;

	WV_REFLECT_TYPE( IEntityComponent, IReflectedType )
public:
	enum class EntityComponentState : int
	{
		FAILED_LOAD = -1,

		UNLOADED = 0,
		LOADING,
		LOADED,
		INITIALIZED
	};

	IEntityComponent()                  : m_ID{ wv::Math::randomU32() } { }
	IEntityComponent( ComponentID _id ) : m_ID{ _id } { }
	
	virtual ~IEntityComponent() { };

	inline ComponentID getID() const { return m_ID; }

	bool isUnloaded()    const { return m_state == EntityComponentState::UNLOADED; }
	bool isLoading()     const { return m_state == EntityComponentState::LOADING; }
	bool isLoaded()      const { return m_state == EntityComponentState::LOADED; }
	bool isInitialized() const { return m_state == EntityComponentState::INITIALIZED; }

protected:
	virtual void load( WorldLoadContext& _ctx ) {
		WV_ASSERT( m_state == EntityComponentState::UNLOADED );

		m_state = EntityComponentState::LOADED;
	}

	virtual void unload( WorldLoadContext& _ctx ) {
		WV_ASSERT( m_state == EntityComponentState::LOADED );

		m_state = EntityComponentState::UNLOADED;
	}

	virtual void initialize() {
		WV_ASSERT( m_state == EntityComponentState::LOADED );

		m_state = EntityComponentState::INITIALIZED;
	}

	virtual void shutdown() {
		WV_ASSERT( m_state == EntityComponentState::INITIALIZED );

		m_state = EntityComponentState::LOADED;
	}

protected:
	ComponentID m_ID = 0;
	EntityComponentState m_state = EntityComponentState::UNLOADED;
};

}