#include "player_controller.h"

#include <wv/input/components/player_input_component.h>
#include <wv/gametest/player_component.h>

#include <wv/application.h>
#include <wv/input/input_system.h>

#include <wv/entity/world.h>

#include <wv/debug/log.h>

void wv::PlayerControllerSystem::configure( ArchetypeConfig& _config )
{
	_config.addComponentType<PlayerControllerComponent>();
}

void wv::PlayerControllerSystem::onInitialize()
{
	InputSystem* inputSystem = wv::Application::getSingleton()->getInputSystem();

	if ( ActionGroup* playerActions = inputSystem->getActionGroup( "Player" ) )
	{
		m_moveActionID = playerActions->getAxisActionID( "Move" );
		m_jumpActionID = playerActions->getTriggerActionID( "Jump" );
	}
}

void wv::PlayerControllerSystem::onUpdate()
{
	/*
	if ( m_playerInput == nullptr || m_player == nullptr )
		return;

	int playerIndex = m_playerInput->getPlayerIndex();
	auto& transform = m_entity->getTransform();

	for ( const ActionEvent& event : _ctx.actionEventQueue )
	{
		if ( event.playerIndex != playerIndex ) 
			continue;

		if ( event.actionID == m_moveActionID )
			m_move = event.action.axis->getValue( playerIndex );
		else if ( event.actionID == m_jumpActionID )
		{
			if ( event.action.trigger->getValue( playerIndex ) && transform.position.y <= 0 )
				m_velocity.y += 5.0f;
		}
	}
	*/
}
