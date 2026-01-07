#include "player_controller.h"

#include <wv/input/components/player_input_component.h>
#include <wv/gametest/player_component.h>

#include <wv/application.h>
#include <wv/input/input_system.h>

#include <wv/entity/world.h>

#include <wv/debug/log.h>

void wv::PlayerControllerSystem::initialize()
{
	InputSystem* inputSystem = wv::Application::getSingleton()->getInputSystem();

	if ( ActionGroup* playerActions = inputSystem->getActionGroup( "Player" ) )
	{
		m_moveActionID = playerActions->getAxisActionID( "Move" );
		m_jumpActionID = playerActions->getTriggerActionID( "Jump" );
	}
}

void wv::PlayerControllerSystem::shutdown()
{

}

void wv::PlayerControllerSystem::registerComponent( IEntityComponent* _component )
{
	if ( PlayerInputComponent* playerInput = tryCast<PlayerInputComponent>( _component ) )
	{
		WV_ASSERT( m_playerInput != nullptr );
		m_playerInput = playerInput;
	}
	
	if ( PlayerControllerComponent* player = tryCast<PlayerControllerComponent>( _component ) )
	{
		WV_ASSERT( m_player != nullptr );
		m_player = player;
	}
}

void wv::PlayerControllerSystem::unregisterComponent( IEntityComponent* _component )
{
	if ( _component == m_playerInput ) m_playerInput = nullptr;
	if ( _component == m_player )      m_player      = nullptr;
}

void wv::PlayerControllerSystem::update( WorldUpdateContext& _ctx )
{
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

	// Physics Update

	m_velocity.x +=  m_move.x;
	m_velocity.z += -m_move.y;

	float yvel = m_velocity.y;
	m_velocity.y = 0;

	// Drag
	m_velocity -= m_velocity * 10.0f * _ctx.deltaTime;

	if ( m_velocity.length() > 10.0f )
		m_velocity.normalize( 1.0f );

	// Gravity
	m_velocity.y = yvel - ( 9.81f * _ctx.deltaTime );

	transform.position += m_velocity * _ctx.deltaTime;
	if ( transform.position.y <= 0 )
	{
		transform.position.y = 0;
		m_velocity.y = 0;
	}
	
	transform.update( nullptr );
}
