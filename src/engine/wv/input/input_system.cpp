#include "input_system.h"

#include <wv/application.h>
#include <wv/event/event_manager.h>
#include <wv/memory/memory.h>

#include <wv/input/drivers/input_driver.h>
#include <wv/input/drivers/controller_driver.h>

#include <SDL2/SDL.h>

wv::InputSystem::InputSystem()
{
	m_playerDeviceMapQueue.push( 0 );
}

wv::InputSystem::~InputSystem()
{
	for ( IInputDriver* driver : m_inputDrivers )
		WV_FREE( driver );

	for ( auto actionGroup : m_actionGroups )
		WV_FREE( actionGroup );

	m_inputDrivers.clear();
	
	m_actionGroups.clear();
	m_actionGroupNameMap.clear();
	m_actionEventQueue.clear();

	m_virtualDevices.clear();
}

void wv::InputSystem::initialize()
{
	for ( IInputDriver* driver : m_inputDrivers )
		driver->initialize( this );
}

void wv::InputSystem::shutdown()
{
	for ( IInputDriver* driver : m_inputDrivers )
		driver->shutdown( this );
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

static wv::ControllerInputs sdlToWvControllerButton( SDL_GameControllerButton _button )
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
		}
	}

	// SDL does this internally, important if a SDL is removed
	//if ( m_hMouseHook != 0 )
	//{
	//	MSG msg{};
	//	while ( PeekMessage( &msg, hwnd, 0, 0, PM_REMOVE) )
	//	{
	//		TranslateMessage( &msg );
	//		DispatchMessage( &msg );
	//	}
	//}
}

void wv::InputSystem::processInputEvents( EventManager* _eventManager )
{
	m_actionEventQueue.clear();
	
	// should be part of platform
	updateInputDrivers( _eventManager );

	for ( IInputDriver* driver : m_inputDrivers )
		driver->pollActions( this );
	
	for ( ActionEvent& action : m_actionEventQueue )
	{
		if ( action.type != ACTION_TYPE_TRIGGER )
			continue;

		if ( !m_playerDeviceMapQueue.empty() )
		{
			int playerID = m_playerDeviceMapQueue.front();
			m_playerDeviceMapQueue.pop();

			setDevicePlayer( action.vdID, playerID );
			wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Mapped device %u to player index %i\n", action.vdID, playerID );
		}
	}
	
#ifndef WV_PACKAGE
	m_debugMouseMotion = { 0.0f, 0.0f };
#endif

}

void wv::InputSystem::setMotorSpeed( uint32_t _vdID, uint16_t _left, uint16_t _right )
{
	IControllerDriver* controllerDriver = nullptr;
	for ( IInputDriver* driver : m_inputDrivers )
	{
		if ( driver->getDriverType() != "Controller" )
			continue;
		controllerDriver = static_cast<IControllerDriver*>( driver );
	}

	if ( controllerDriver == nullptr )
		return;
	
	controllerDriver->setMotorSpeed( _vdID, _left, _right, 0 );
}

void wv::InputSystem::setDevicePlayer( uint32_t _vdID, int _playerIndex )
{
	if ( _playerIndex < -1 ) 
		_playerIndex = -1;
	
	bool requiresNew = true;
	int prevPlayerIndex = -1;

	for ( auto& device : m_virtualDevices )
	{
		if ( device.virtualDeviceID != _vdID )
			continue;
		
		prevPlayerIndex = device.playerIndex;
		device.playerIndex = _playerIndex;
		requiresNew = false;
		break;
	}

	// create new virtual device
	if( requiresNew )
		m_virtualDevices.emplace_back( _vdID, _playerIndex );

	for ( auto& action : m_actionEventQueue )
	{
		if ( action.vdID != _vdID )
			continue;

		if ( action.playerIndex == _playerIndex )
			continue;

		action.playerIndex = _playerIndex;
		
		switch ( action.type )
		{
		case ACTION_TYPE_TRIGGER: action.action.trigger->setValue( _playerIndex, action.action.trigger->getValue( prevPlayerIndex ) ); break;
		case ACTION_TYPE_VALUE:   action.action.value  ->setValue( _playerIndex, action.action.value  ->getValue( prevPlayerIndex ) ); break;
		case ACTION_TYPE_AXIS:    action.action.axis   ->setValue( _playerIndex, action.action.axis   ->getValue( prevPlayerIndex ) ); break;
		}
	}
}

int wv::InputSystem::getDevicePlayer( uint32_t _vdID )
{
	for ( auto& device : m_virtualDevices )
	{
		if ( device.virtualDeviceID != _vdID )
			continue;

		return device.playerIndex;
	}

	return -1;
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
