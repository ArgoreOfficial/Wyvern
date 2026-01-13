#pragma once

#include <wv/reflection/reflection.h>
#include <wv/math/math.h>


namespace wv {

typedef uint32_t ComponentID;

struct WorldLoadContext;

class IEntityComponent : public wv::IReflectedType
{
	friend class Entity;

	WV_REFLECT_TYPE( IEntityComponent )
public:
	enum class EntityComponentState : int
	{
		FAILED_LOAD = -1,

		UNLOADED = 0,
		LOADING,
		LOADED,
		INITIALIZED
	};

	IEntityComponent() = default;
	virtual ~IEntityComponent() { };

	inline ComponentID getID() const { return m_ID; }

protected:
	virtual void load( WorldLoadContext& _ctx ) {
		WV_ASSERT( m_state != EntityComponentState::UNLOADED );

		m_state = EntityComponentState::LOADED;
	}

	virtual void unload( WorldLoadContext& _ctx ) {
		WV_ASSERT( m_state != EntityComponentState::LOADED );

		m_state = EntityComponentState::UNLOADED;
	}

	virtual void initialize() {
		WV_ASSERT( m_state != EntityComponentState::LOADED );

		m_state = EntityComponentState::INITIALIZED;
	}

	virtual void shutdown() {
		WV_ASSERT( m_state != EntityComponentState::INITIALIZED );

		m_state = EntityComponentState::LOADED;
	}

protected:
	const ComponentID m_ID = wv::Math::randomU32();
	EntityComponentState m_state = EntityComponentState::UNLOADED;
};

}