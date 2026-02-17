#include "windows_mouse_driver.h"

#include <wv/application.h>
#include <sdl/sdl_display_driver.h>

#include <windows/windows.h>

void wv::WindowsMouseDriver::initialize( InputSystem* _inputSystem )
{
	IMouseDriver::initialize( _inputSystem );

}

void wv::WindowsMouseDriver::shutdown( InputSystem* _inputSystem )
{
	IMouseDriver::shutdown( _inputSystem );
}

void wv::WindowsMouseDriver::pollActions( InputSystem* _inputSystem )
{
	SDLDisplayDriver* displayDriver = (SDLDisplayDriver*)wv::Application::getSingleton()->getDisplayDriver();
	HWND hwnd = displayDriver->getWindowHandle();

	bool lrFlipped = GetSystemMetrics( SM_SWAPBUTTON );
	
	m_prevState = m_state;

	m_state.scrollDelta = 0;
	m_state.motion = {};

	for ( const LowLevelInputQueue::Event& event : _inputSystem->getLowLevelQueue().getQueue() )
	{
		switch ( event.type )
		{
		case LowLevelInputQueue::WV_MOUSE_MOVE:
			m_state.position = event.mouse.move.position;
			m_state.motion   = event.mouse.move.delta;
			break;

		case LowLevelInputQueue::WV_MOUSE_BUTTON:
		{
			int buttonIndex = event.mouse.button.index;
			if ( lrFlipped )
			{
				if ( buttonIndex == MOUSE_BUTTON_LEFT )
					buttonIndex = MOUSE_BUTTON_RIGHT;
				else if ( buttonIndex == MOUSE_BUTTON_RIGHT )
					buttonIndex = MOUSE_BUTTON_LEFT;
			}

			m_state.buttonStates[ buttonIndex ] = event.mouse.button.state;
		} break;

		case LowLevelInputQueue::WV_MOUSE_SCROLL:
			m_state.scrollDelta += event.mouse.scroll.delta;
			break;
		}
	}

	sendTriggerEvents( _inputSystem );
	sendValueEvents( _inputSystem );
	sendAxisEvents( _inputSystem );
}
