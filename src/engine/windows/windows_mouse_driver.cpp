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
		case LowLevelInputQueue::EventType_Move:
			m_state.position = event.mouse.move.position;
			m_state.motion   = event.mouse.move.delta;
			break;

		case LowLevelInputQueue::EventType_Button:
		{
			int buttonIndex = event.mouse.button.index;
			if ( lrFlipped )
			{
				if ( buttonIndex == MouseInput_Left )
					buttonIndex = MouseInput_Right;
				else if ( buttonIndex == MouseInput_Right )
					buttonIndex = MouseInput_Left;
			}

			m_state.buttonStates[ buttonIndex ] = event.mouse.button.state;
		} break;

		case LowLevelInputQueue::EventType_Scroll:
			m_state.scrollDelta += event.mouse.scroll.delta;
			break;
		}
	}

	sendTriggerEvents( _inputSystem );
	sendValueEvents( _inputSystem );
	sendAxisEvents( _inputSystem );
}
