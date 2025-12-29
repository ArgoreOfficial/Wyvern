#include "input_system.h"

#include <wv/application.h>
#include <wv/event/event_manager.h>
#include <wv/memory/memory.h>

#include <SDL2/SDL.h>

wv::InputSystem::InputSystem() :
	WV_BIND_EVENT_FUNCTION( m_mouseMoveListener, onMouseMoveEvent ),
	WV_BIND_EVENT_FUNCTION( m_mouseButtonListener, onMouseButtonEvent ),
	WV_BIND_EVENT_FUNCTION( m_keyboardListener, onKeyboardEvent ),
	WV_BIND_EVENT_FUNCTION( m_controllerButtonListener, onControllerButtonEvent )
{
	EventManager* eventManager = wv::Application::getSingleton()->getEventManager();

	eventManager->subscribe( &m_mouseMoveListener );
	eventManager->subscribe( &m_mouseButtonListener );
	eventManager->subscribe( &m_keyboardListener );
	eventManager->subscribe( &m_controllerButtonListener );
}

wv::InputSystem::~InputSystem()
{
	for ( auto actionGroup : m_actionGroups )
		WV_FREE( actionGroup );

	m_actionGroups.clear();
	m_actionGroupNameMap.clear();
}

static SDL_GameController* findController() {
	for ( int i = 0; i < SDL_NumJoysticks(); i++ )
	{
		if ( SDL_IsGameController( i ) )
		{
			return SDL_GameControllerOpen( i );
		}
	}

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
	case SDL_CONTROLLER_BUTTON_INVALID:       return wv::ControllerButton::NONE;           break;
	case SDL_CONTROLLER_BUTTON_A:             return wv::ControllerButton::A;              break;
	case SDL_CONTROLLER_BUTTON_B:             return wv::ControllerButton::B;              break;
	case SDL_CONTROLLER_BUTTON_X:             return wv::ControllerButton::X;              break;
	case SDL_CONTROLLER_BUTTON_Y:             return wv::ControllerButton::Y;              break;
	case SDL_CONTROLLER_BUTTON_BACK:          return wv::ControllerButton::SELECT;         break;
	case SDL_CONTROLLER_BUTTON_GUIDE:         return wv::ControllerButton::HOME;           break;
	case SDL_CONTROLLER_BUTTON_START:         return wv::ControllerButton::START;          break;
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:     return wv::ControllerButton::JOYSTICK_LEFT;  break;
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK:    return wv::ControllerButton::JOYSTICK_RIGHT; break;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return wv::ControllerButton::SHOULDER_LEFT;  break;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return wv::ControllerButton::SHOULDER_RIGHT; break;
	case SDL_CONTROLLER_BUTTON_DPAD_UP:       return wv::ControllerButton::DPAD_UP;        break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:     return wv::ControllerButton::DPAD_DOWN;      break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:     return wv::ControllerButton::DPAD_LEFT;      break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:    return wv::ControllerButton::DPAD_RIGHT;     break;
	}

	return wv::ControllerButton::NONE;
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

		case SDL_CONTROLLERBUTTONDOWN: [[fallthrough]];
		case SDL_CONTROLLERBUTTONUP:
		{
			ControllerButtonEvent event;
			event.state = ev.type == SDL_CONTROLLERBUTTONDOWN;
			event.button = sdlToWvControllerButton( (SDL_GameControllerButton)ev.cbutton.button );
			_eventManager->queueEvent( event );
		} break;

		case SDL_CONTROLLERDEVICEADDED:
			if ( !controller )
			{
				wv::Debug::Print( "Controller connected\n" );
				WV_LOG_ERROR( "TODO: proper controller handling\n" );
				controller = SDL_GameControllerOpen( ev.cdevice.which );
			}
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			if ( controller && ev.cdevice.which == SDL_JoystickInstanceID( SDL_GameControllerGetJoystick( controller ) ) )
			{
				wv::Debug::Print( "Controller disconnected\n" );
				
				SDL_GameControllerClose( controller );
				controller = findController();
			}
			break;
		}
	}
}

void wv::InputSystem::processInputEvents( EventManager* _eventManager )
{
	// should be part of platform
	updateInputDrivers( _eventManager );

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

void wv::InputSystem::onMouseMoveEvent( const MouseMoveEvent& _event )
{
#ifndef WV_PACKAGE
	m_debugMouseMotion   = _event.move;
	m_debugMousePosition = _event.position;
#endif
}

void wv::InputSystem::onMouseButtonEvent( const MouseButtonEvent& _event )
{
	WV_ASSERT( _event.buttonID >= 5 );
#ifndef WV_PACKAGE
	m_debugMouseButtonStates[ _event.buttonID ] = _event.state;
#endif
}

void wv::InputSystem::onKeyboardEvent( const KeyboardEvent& _event )
{
	if ( !_event.isRepeat )
		for ( auto group : m_actionGroups )
			group->handleKeyboardEvent( _event.scancode, _event.state );
}

void wv::InputSystem::onControllerButtonEvent( const ControllerButtonEvent& _event )
{
	for ( auto group : m_actionGroups )
		group->handleControllerEvent( _event.button, _event.state );
}
