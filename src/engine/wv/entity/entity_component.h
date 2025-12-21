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
		if ( m_state != EntityComponentState::UNLOADED ) return; // assert
		m_state = EntityComponentState::LOADED;
	}

	virtual void unload() {
		if ( m_state != EntityComponentState::LOADED ) return; // assert
		m_state = EntityComponentState::UNLOADED;
	}

	virtual void initialize() {
		if ( m_state != EntityComponentState::LOADED ) return; // assert
		m_state = EntityComponentState::INITIALIZED;
	}

	virtual void shutdown() {
		if ( m_state != EntityComponentState::INITIALIZED ) return; // assert
		m_state = EntityComponentState::LOADED;
	}

protected:
	EntityComponentState m_state = EntityComponentState::UNLOADED;
};

}