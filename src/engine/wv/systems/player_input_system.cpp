#include "player_input_system.h"

#include <wv/components/player_input_component.h>

#include <wv/application.h>
#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/debug/log.h>
#include <wv/input/input_system.h>

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

	Debug::Print( Debug::PrintLevel_Debug, "Joined Player %i\n", _playerIndex );

	updateNextAvailableIndex();
}

void wv::PlayerInputSystem::disconnectDevice( uint32_t _vDeviceID )
{
	if ( _vDeviceID == 0 )
		return; // device id invalid

	int playerIndex = m_inputSystem->getDevicePlayer( _vDeviceID );
	if ( playerIndex == -1 || !m_activePlayers.contains( playerIndex ) )
		return; // player not connected

	disconnect( _vDeviceID, playerIndex );
}

void wv::PlayerInputSystem::disconnectPlayer( int _playerIndex )
{
	if ( _playerIndex == -1 || !m_activePlayers.contains( _playerIndex ) )
		return; // player not connected

	uint32_t vDeviceID = m_inputSystem->getPlayerDevice( _playerIndex );
	if ( vDeviceID == 0 )
		return; // device id invalid

	disconnect( vDeviceID, _playerIndex );
}

void wv::PlayerInputSystem::clearPlayers()
{
	auto players = m_activePlayers; // create temp copy

	for ( auto p : players )
		disconnectPlayer( p );
	
	WV_ASSERT( m_activePlayers.size() == 0 );
}

void wv::PlayerInputSystem::configure( ArchetypeConfig& _config )
{
	setUpdateMode( UpdateMode_Always );

	_config.addComponentType<PlayerInputComponent>();
}

void wv::PlayerInputSystem::onInitialize()
{
	if ( m_inputSystem == nullptr )
		m_inputSystem = wv::Application::getSingleton()->getInputSystem();
}

void wv::PlayerInputSystem::onUpdate()
{
	if ( getWorld()->isEditorState() )
	{
		if ( !m_activePlayers.empty() )
			clearPlayers();

		return;
	}

	while ( m_activePlayers.contains( m_nextAvailableIndex ) )
		m_nextAvailableIndex++;

	// Handle disconnect
	for ( ActionEvent& action : updateContext->actionEventQueue )
	{
		if ( action.type != ActionType_DeviceDisconnected )
			continue;

		disconnectDevice( action.vdID );
	}

	if ( m_maxPlayers != -1 && m_nextAvailableIndex > m_maxPlayers )
		return; // max players reached, skip

	for ( ActionEvent& action : updateContext->actionEventQueue )
	{
		// TODO: any (unmapped or mapped) button
		if ( action.type != ActionType_Trigger ) // must be a trigger action
			continue;

		if ( action.playerIndex != -1 ) // must be an unused device
			continue;

		if ( action.action.trigger->getValue( -1 ) == false ) // must be a press, not a release
			continue;

		if ( m_selectionMode == SelectionMode::SelectionMode_AnyTriggerAction )
			joinPlayer( action.vdID );
		else if ( m_selectionMode == SelectionMode::SelectionMode_SpecificTriggerAction )
		{
			if ( action.actionID == m_joinActionID )
				joinPlayer( action.vdID );
		}
	}

}

void wv::PlayerInputSystem::disconnect( uint32_t _vDeviceID, int _playerIndex )
{
	m_activePlayers.erase( _playerIndex );
	m_inputSystem->setDevicePlayer( _vDeviceID, -1 );

	Debug::Print( Debug::PrintLevel_Debug, "Disconnected Player %i\n", _playerIndex );

	updateNextAvailableIndex();
}
