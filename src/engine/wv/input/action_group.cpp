#include "action_group.h"

#include <wv/memory/memory.h>
#include <wv/debug/log.h>

#include <wv/input/actions/button_action.h>

wv::ActionGroup::~ActionGroup()
{
	for ( auto action : m_actions )
		WV_FREE( action );

	m_actions.clear();
	m_actionNameMap.clear();
	m_keyboardBoundActions.clear();
	m_controllerBoundActions.clear();
	m_mouseBoundActions.clear();
}

void wv::ActionGroup::enable()
{
	WV_ASSERT( m_isEnabled == true );
	m_isEnabled = true;

	// check if any action have changed
	if ( !m_requiresRemapping )
	{
		for ( auto action : m_actions )
		{
			if ( !action->requiresRemapping() ) continue;
			m_requiresRemapping = true;
			break;
		}
	}

	// check if action mapping requires rebuilding
	if ( m_requiresRemapping )
	{
		m_keyboardBoundActions.clear();
		m_controllerBoundActions.clear();
		m_mouseBoundActions.clear();

		buildActionMapping();

		m_requiresRemapping = false;
	}
}

wv::ButtonAction* wv::ActionGroup::createButtonAction( const std::string& _name )
{
	WV_ASSERT_MSG( m_isEnabled == true, "Enabled action groups cannot be modified" );

	if ( m_actionNameMap.contains( _name ) && m_actionNameMap.at( _name ) != nullptr )
	{
		WV_LOG_WARNING( "ActionGroup '%s' already contains '%s', a new one will not be created\n", m_name.c_str(), _name.c_str() );
		return (ButtonAction*)m_actionNameMap.at( _name );
	}

	ButtonAction* action = WV_NEW( ButtonAction, _name );
	m_actions.push_back( action );
	m_actionNameMap.emplace( _name, action );
	m_requiresRemapping = true;
	return action;
}

void wv::ActionGroup::destroyAction( const std::string& _name )
{
	WV_ASSERT_MSG( m_isEnabled == true, "Enabled action groups cannot be modified" );

	if ( !m_actionNameMap.contains( _name ) )
	{
		WV_LOG_WARNING( "Cannot delete action '%s' in action group '%s', it does not exist\n", _name.c_str(), m_name.c_str() );
		return;
	}

	IAction* action = m_actionNameMap.at( _name );
	m_actionNameMap.erase( _name );

	for ( auto it = m_actions.begin(); it != m_actions.end(); )
	{
		if ( *it != action )
			continue;

		m_actions.erase( it );
		break;
	}

	WV_FREE( action );
	m_requiresRemapping = true;
}

void wv::ActionGroup::buildActionMapping()
{
	for ( IAction* action : m_actions )
	{
		if ( action->isBoundToKeyboard() )   m_keyboardBoundActions.push_back( action );
		if ( action->isBoundToController() ) m_controllerBoundActions.push_back( action );
		if ( action->isBoundToMouse() )      m_mouseBoundActions.push_back( action );
	}
}

void wv::ActionGroup::handleKeyboardEvent( uint32_t _scancode, bool _keyDown )
{
	if ( !isEnabled() ) return;

	for ( IAction* action : m_keyboardBoundActions )
		action->handleKeyboardEvent( _scancode, _keyDown );
}

void wv::ActionGroup::handleControllerEvent( uint32_t _button, bool _buttonDown )
{
	if ( !isEnabled() ) return;

	for ( IAction* action : m_controllerBoundActions )
		action->handleControllerEvent( _button, _buttonDown );
}