#include "action_group.h"

#include <wv/memory/memory.h>
#include <wv/debug/log.h>

#include <wv/input/actions/button_action.h>
#include <wv/input/actions/axis_action.h>

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
	return static_cast<ButtonAction*>( createAction( WV_NEW( ButtonAction, _name ) ) );
}

wv::AxisAction* wv::ActionGroup::createAxisAction( const std::string& _name )
{
	WV_ASSERT_MSG( m_isEnabled == true, "Enabled action groups cannot be modified" );
	return static_cast<AxisAction*>( createAction( WV_NEW( AxisAction, _name ) ) );
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

wv::IAction* wv::ActionGroup::createAction( IAction* _action )
{
	std::string name = _action->m_name;

	if ( m_actionNameMap.contains( name ) && m_actionNameMap.at( name ) != nullptr )
	{
		WV_LOG_WARNING( "ActionGroup '%s' already contains an action named '%s', a new one will not be created\n", m_name.c_str(), name.c_str() );
		WV_FREE( _action );
		return m_actionNameMap.at( name );
	}

	m_actions.push_back( _action );
	m_actionNameMap.emplace( name, _action );
	m_requiresRemapping = true;
	return _action;
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

void wv::ActionGroup::handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown )
{
	if ( !isEnabled() ) return;

	for ( IAction* action : m_keyboardBoundActions )
		action->handleKeyboardEvent( _scancode, _keyDown );
}

void wv::ActionGroup::handleControllerEvent( wv::ControllerButton _button, bool _buttonDown )
{
	if ( !isEnabled() ) return;

	for ( IAction* action : m_controllerBoundActions )
		action->handleControllerEvent( _button, _buttonDown );
}

wv::Vector2<double> wv::ActionGroup::getAxisValue( const std::string& _name ) const
{
	if ( !m_actionNameMap.contains( _name ) )
	{
		WV_LOG_ERROR( "Action group %s does not contain axis action '%s'\n", m_name.c_str(), _name.c_str() );
		return { 0.0, 0.0 };
	}

	if ( AxisAction* action = tryCast<AxisAction>( m_actionNameMap.at( _name ) ) )
		return action->getValue();

	WV_LOG_ERROR( "Action '%s' in group '%s' is not an AxisAction\n", m_name.c_str(), _name.c_str() );
	return { 0.0, 0.0 };
}
