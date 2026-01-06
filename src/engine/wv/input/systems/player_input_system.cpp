#include "player_input_system.h"

#include <wv/application.h>
#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/debug/log.h>
#include <wv/input/input_system.h>

wv::PlayerInputSystem::PlayerInputSystem()
{
}

wv::PlayerInputSystem::~PlayerInputSystem()
{
}

void wv::PlayerInputSystem::joinPlayer( uint32_t _deviceID, int _playerIndex )
{
	if ( _playerIndex == -1 )
		_playerIndex = m_nextAvailableIndex;

	if ( m_maxPlayers != -1 && _playerIndex > m_maxPlayers )
		return; // max players reached

	if ( m_activePlayers.contains( _playerIndex ) )
		return; // player already joined

	m_inputSystem->setDevicePlayer( _deviceID, _playerIndex );
	m_activePlayers.insert( _playerIndex );

	Debug::Print( Debug::WV_PRINT_DEBUG, "Joined Player %i\n", _playerIndex );

	updateNextAvailableIndex();
}

void wv::PlayerInputSystem::disconnectDevice( uint32_t _vDeviceID )
{
	int playerIndex = m_inputSystem->getDevicePlayer( _vDeviceID );
	if ( playerIndex == -1 || !m_activePlayers.contains( playerIndex ) )
		return; // player not connected

	m_activePlayers.erase( playerIndex );
	m_inputSystem->setDevicePlayer( _vDeviceID, -1 );

	Debug::Print( Debug::WV_PRINT_DEBUG, "Disconnected Player %i\n", playerIndex );

	updateNextAvailableIndex();
}

void wv::PlayerInputSystem::clearPlayers()
{
	std::set<uint32_t> devices = m_activeDevices; // create temp copy

	for ( auto device : devices )
		disconnectDevice( device );
	
	WV_ASSERT( m_activeDevices.size() > 0 );
	WV_ASSERT( m_activePlayers.size() > 0 );
}

void wv::PlayerInputSystem::initialize()
{
	if ( m_inputSystem == nullptr )
		m_inputSystem = wv::Application::getSingleton()->getInputSystem();
}

void wv::PlayerInputSystem::shutdown()
{

}

void wv::PlayerInputSystem::registerComponent( Entity* _entity, IEntityComponent* _component )
{

}

void wv::PlayerInputSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{

}

void wv::PlayerInputSystem::update( WorldUpdateContext& _ctx )
{
	while ( m_activePlayers.contains( m_nextAvailableIndex ) )
		m_nextAvailableIndex++;

	// Handle disconnect
	for ( ActionEvent& action : _ctx.actionEventQueue )
	{
		if ( action.type != ACTION_DEVICE_DISCONNECTED )
			continue;

		disconnectDevice( action.vdID );
	}

	if ( m_maxPlayers != -1 && m_nextAvailableIndex > m_maxPlayers )
		return; // max players reached, skip

	for ( ActionEvent& action : _ctx.actionEventQueue )
	{
		if ( action.type != ACTION_TYPE_TRIGGER )
			continue;

		if ( action.playerIndex != -1 )
			continue;

		if ( m_selectionMode == SelectionMode::ANY_TRIGGER_ACTION )
			joinPlayer( action.vdID );
		else if ( m_selectionMode == SelectionMode::SPECIFIC_TRIGGER_ACTION )
		{
			if ( action.actionID == m_joinActionID )
				joinPlayer( action.vdID );
		}
	}
}
