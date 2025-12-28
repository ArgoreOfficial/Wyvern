#include "input_system.h"

#include <wv/application.h>

#include <SDL2/SDL.h>

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
			break;

		case SDL_EventType::SDL_KEYUP:
			diEvent.eventType = DriverInputEventType::KEY_UP;
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
