#pragma once

#include <wv/entity/world_system.h>
#include <wv/entity/entity_component.h>

#include <wv/input/action.h>

#include <wv/math/vector2.h>

#include <vector>
#include <set>

namespace wv {

class PlayerInputComponent;
class InputSystem;

class PlayerInputSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( PlayerInputSystem, IWorldSystem )
public:
	enum class SelectionMode
	{
		ANY_TRIGGER_ACTION = 0,
		SPECIFIC_TRIGGER_ACTION
	};

	PlayerInputSystem();
	~PlayerInputSystem();

	void setSelectionMode( SelectionMode _mode ) { m_selectionMode = _mode; }
	void setActionID( ActionID _actionID ) { m_joinActionID = _actionID; }
	void setMaxPlayers( int _maxPlayers ) { m_maxPlayers = wv::Math::max( -1, _maxPlayers ); }

	void joinPlayer( uint32_t _deviceID, int _playerIndex = -1 );
	void disconnectDevice( uint32_t _deviceID );

	void clearPlayers();

protected:
	virtual void initialize() override;
	virtual void shutdown() override;

	virtual void registerComponent( Entity* _entity, IEntityComponent* _component ) override;
	virtual void unregisterComponent( Entity* _entity, IEntityComponent* _component ) override;

	void update( WorldUpdateContext& _ctx ) override;

	void updateNextAvailableIndex() {
		m_nextAvailableIndex = 0;
		while ( m_activePlayers.contains( m_nextAvailableIndex ) )
			m_nextAvailableIndex++;
	}

	InputSystem* m_inputSystem = nullptr;

	SelectionMode m_selectionMode = SelectionMode::ANY_TRIGGER_ACTION;
	ActionID m_joinActionID = 0;

	int m_maxPlayers = -1; // -1 is unlimited
	int m_nextAvailableIndex = 0;
	std::set<int> m_activePlayers;
	std::set<uint32_t> m_activeDevices;
};

}