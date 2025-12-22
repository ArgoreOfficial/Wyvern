#pragma once

#include <wv/reflection/reflection.h>

namespace wv {

class IEntityComponent : wv::IReflectedType
{
	WV_REFLECT_TYPE( IEntityComponent )
public:
	enum class EntityComponentState : uint8_t
	{
		UNLOADED,
		LOADED,
		INITIALIZED
	};

	IEntityComponent() = default;

	virtual void load() { 
		WV_ASSERT( m_state != EntityComponentState::UNLOADED );

		m_state = EntityComponentState::LOADED;
	}

	virtual void unload() {
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
	EntityComponentState m_state = EntityComponentState::UNLOADED;
};

}