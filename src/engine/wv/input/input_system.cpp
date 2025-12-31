#include "input_system.h"

#include <wv/application.h>
#include <wv/event/event_manager.h>
#include <wv/memory/memory.h>
#include <wv/input/input_events.h>

#include <wv/input/drivers/input_driver.h>

#include <SDL2/SDL.h>

wv::InputSystem::InputSystem()
{

}

wv::InputSystem::~InputSystem()
{
	for ( auto actionGroup : m_actionGroups )
		WV_FREE( actionGroup );

	m_actionGroups.clear();
	m_actionGroupNameMap.clear();

	for ( IInputDriver* driver : m_inputDrivers )
		WV_FREE( driver );
}

static SDL_GameController* findController() {
	for ( int i = 0; i < SDL_NumJoysticks(); i++ )
		if ( SDL_IsGameController( i ) )
			return SDL_GameControllerOpen( i );
	
	return nullptr;
}

static wv::Scancode sdlToWvScancode( SDL_Scancode _scancode )
{
	// identical
	return static_cast<wv::Scancode>( _scancode );
}

static wv::ControllerButton sdlToWvControllerButton( SDL_GameControllerButton _button )
{
	switch ( _button )
	{
	case SDL_CONTROLLER_BUTTON_INVALID:       return wv::CONTROLLER_BUTTON_NONE;           break;
	case SDL_CONTROLLER_BUTTON_A:             return wv::CONTROLLER_BUTTON_A;              break;
	case SDL_CONTROLLER_BUTTON_B:             return wv::CONTROLLER_BUTTON_B;              break;
	case SDL_CONTROLLER_BUTTON_X:             return wv::CONTROLLER_BUTTON_X;              break;
	case SDL_CONTROLLER_BUTTON_Y:             return wv::CONTROLLER_BUTTON_Y;              break;
	case SDL_CONTROLLER_BUTTON_BACK:          return wv::CONTROLLER_BUTTON_SELECT;         break;
	case SDL_CONTROLLER_BUTTON_GUIDE:         return wv::CONTROLLER_BUTTON_HOME;           break;
	case SDL_CONTROLLER_BUTTON_START:         return wv::CONTROLLER_BUTTON_START;          break;
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:     return wv::CONTROLLER_BUTTON_JOYSTICK_LEFT;  break;
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK:    return wv::CONTROLLER_BUTTON_JOYSTICK_RIGHT; break;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return wv::CONTROLLER_BUTTON_SHOULDER_LEFT;  break;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return wv::CONTROLLER_BUTTON_SHOULDER_RIGHT; break;
	case SDL_CONTROLLER_BUTTON_DPAD_UP:       return wv::CONTROLLER_BUTTON_DPAD_UP;        break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:     return wv::CONTROLLER_BUTTON_DPAD_DOWN;      break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:     return wv::CONTROLLER_BUTTON_DPAD_LEFT;      break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:    return wv::CONTROLLER_BUTTON_DPAD_RIGHT;     break;
	}

	return wv::CONTROLLER_BUTTON_NONE;
}

static SDL_GameController* controller = nullptr;

void wv::InputSystem::updateInputDrivers( EventManager* _eventManager )
{
	Application* app = Application::getSingleton();

	SDL_Event ev;
	while ( SDL_PollEvent( &ev ) )
	{
		switch ( ev.type )
		{
		case SDL_QUIT: app->quit(); break;
		//case SDL_EventType::SDL_WINDOWEVENT: windowCallback( m_windowContext, &ev.window ); break;

		case SDL_KEYDOWN: [[fallthrough]];
		case SDL_KEYUP:
		{
			KeyboardEvent event;
			event.state = ev.type == SDL_EventType::SDL_KEYDOWN;
			event.isRepeat = ev.key.repeat;
			event.scancode = sdlToWvScancode( ev.key.keysym.scancode );
			_eventManager->queueEvent( event );
		} break;

		case SDL_MOUSEBUTTONDOWN: [[fallthrough]];
		case SDL_MOUSEBUTTONUP:
		{
			MouseButtonEvent event;
			event.buttonID = ev.button.button;
			event.state = ev.type == SDL_MOUSEBUTTONDOWN;
			_eventManager->queueEvent( event );
		} break;

		case SDL_MOUSEMOTION:
		{
			MouseMoveEvent event;
			event.move = { (float)ev.motion.xrel, (float)ev.motion.yrel };
			event.position = { ev.motion.x, ev.motion.y };
			_eventManager->queueEvent( event );
		} break;

		//case SDL_CONTROLLERBUTTONDOWN: [[fallthrough]];
		//case SDL_CONTROLLERBUTTONUP:
		//{
		//	ControllerButtonEvent event;
		//	event.state = ev.type == SDL_CONTROLLERBUTTONDOWN;
		//	event.button = sdlToWvControllerButton( (SDL_GameControllerButton)ev.cbutton.button );
		//	_eventManager->queueEvent( event );
		//} break;
		//
		//case SDL_CONTROLLERDEVICEADDED:
		//	if ( !controller )
		//	{
		//		wv::Debug::Print( "Controller connected\n" );
		//		WV_LOG_ERROR( "TODO: proper controller handling\n" );
		//		controller = SDL_GameControllerOpen( ev.cdevice.which );
		//	}
		//	break;
		//case SDL_CONTROLLERDEVICEREMOVED:
		//	if ( controller && ev.cdevice.which == SDL_JoystickInstanceID( SDL_GameControllerGetJoystick( controller ) ) )
		//	{
		//		wv::Debug::Print( "Controller disconnected\n" );
		//		
		//		SDL_GameControllerClose( controller );
		//		controller = findController();
		//	}
		//	break;
		}
	}
}

void wv::InputSystem::processInputEvents( EventManager* _eventManager )
{
	m_actionEventQueue.clear();
	
	// should be part of platform
	updateInputDrivers( _eventManager );

	for ( IInputDriver* driver : m_inputDrivers )
		driver->updateDriver( this );
	
#ifndef WV_PACKAGE
	m_debugMouseMotion = { 0.0f, 0.0f };
#endif

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
