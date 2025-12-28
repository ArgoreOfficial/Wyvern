#include "input_system.h"

#include <wv/application.h>
#include <wv/memory/memory.h>

#include <SDL2/SDL.h>

wv::InputSystem::~InputSystem()
{
	for ( auto actionGroup : m_actionGroups )
		WV_FREE( actionGroup );

	m_actionGroups.clear();
	m_actionGroupNameMap.clear();
}

void wv::InputSystem::updateInputDrivers()
{
	Application* app = Application::getSingleton();

	SDL_Event ev;
	while ( SDL_PollEvent( &ev ) )
	{
		DriverInputEvent diEvent{ DriverInputEventType::UNUSUED };

		switch ( ev.type )
		{
		case SDL_EventType::SDL_QUIT: app->quit(); break;
		//case SDL_EventType::SDL_WINDOWEVENT: windowCallback( m_windowContext, &ev.window ); break;

		case SDL_EventType::SDL_KEYDOWN:
			diEvent.eventType = DriverInputEventType::KEY_DOWN;
			for ( auto group : m_actionGroups )
				group->handleKeyboardEvent( ev.key.keysym.scancode, true );
			break;

		case SDL_EventType::SDL_KEYUP:
			diEvent.eventType = DriverInputEventType::KEY_UP;
			for ( auto group : m_actionGroups )
				group->handleKeyboardEvent( ev.key.keysym.scancode, false );
			break;

		case SDL_EventType::SDL_MOUSEBUTTONDOWN:
			diEvent.eventType = DriverInputEventType::MOUSE_DOWN;
			diEvent.mouseButtonID = ev.button.button;
			break;

		case SDL_EventType::SDL_MOUSEBUTTONUP:
			diEvent.eventType = DriverInputEventType::MOUSE_UP;
			diEvent.mouseButtonID = ev.button.button;
			break;

		case SDL_EventType::SDL_MOUSEMOTION:
			diEvent.eventType = DriverInputEventType::MOUSE_MOVE;
			diEvent.mouseMotion.x = (float)ev.motion.xrel;
			diEvent.mouseMotion.y = (float)ev.motion.yrel;
			diEvent.mousePosition.x = (float)ev.motion.x;
			diEvent.mousePosition.y = (float)ev.motion.y;
			break;
		}

		if ( diEvent.eventType != DriverInputEventType::UNUSUED )
			m_driverEvents.push_back( diEvent );
	}
}

void wv::InputSystem::processInputEvents()
{
	// should be part of platform
	updateInputDrivers();

#ifndef WV_PACKAGE
	m_debugMouseMotion = { 0.0f, 0.0f };
#endif

	for ( auto& ev : m_driverEvents )
	{
		WV_ASSERT( ev.eventType == DriverInputEventType::UNUSUED );

		switch ( ev.eventType )
		{
		case DriverInputEventType::MOUSE_MOVE:
		#ifndef WV_PACKAGE
			m_debugMouseMotion = ev.mouseMotion;
			m_debugMousePosition = ev.mousePosition;
		#endif
			break;

		case DriverInputEventType::MOUSE_UP:
			WV_ASSERT( ev.mouseButtonID >= 5 );
		#ifndef WV_PACKAGE
			m_debugMouseButtonStates[ ev.mouseButtonID ] = false;
		#endif
			break;

		case DriverInputEventType::MOUSE_DOWN:
			WV_ASSERT( ev.mouseButtonID >= 5 );
		#ifndef WV_PACKAGE
			m_debugMouseButtonStates[ ev.mouseButtonID ] = true;
		#endif
			break;
		}
	}

	m_driverEvents.clear();
}

wv::ActionGroup* wv::InputSystem::createActionGroup( const std::string& _name )
{
	if ( m_actionGroupNameMap.contains( _name ) && m_actionGroupNameMap.at( _name ) != nullptr )
	{
		WV_LOG_WARNING( "ActionGroup '%s' already exists, a new one will not be created\n", _name.c_str() );
		return m_actionGroupNameMap.at( _name );
	}

	ActionGroup* group = WV_NEW( ActionGroup, _name );
	m_actionGroups.push_back( group );
	m_actionGroupNameMap.emplace( _name, group );
	return group;
}

void wv::InputSystem::destroyActionGroup( const std::string& _name )
{
	if ( !m_actionGroupNameMap.contains( _name ) )
	{
		WV_LOG_WARNING( "Cannot delete action group '%s'. it does not exist\n", _name.c_str() );
		return;
	}

	ActionGroup* group = m_actionGroupNameMap.at( _name );
	m_actionGroupNameMap.erase( _name );

	for ( auto it = m_actionGroups.begin(); it != m_actionGroups.end(); )
	{
		if ( *it != group )
			continue;

		m_actionGroups.erase( it );
		break;
	}

	WV_FREE( group );
}
