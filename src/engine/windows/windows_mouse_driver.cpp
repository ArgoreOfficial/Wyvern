#include "windows_mouse_driver.h"

#include <wv/application.h>
#include <sdl/sdl_display_driver.h>

#include <windows/windows.h>

enum WindowsMouseEventType
{
	WIN_MOUSE_EVENT_NONE = 0,
	WIN_MOUSE_EVENT_SCROLL,
	WIN_MOUSE_EVENT_MOVE
};

struct WindowsMouseEvent
{
	enum WindowsMouseEventType type = WIN_MOUSE_EVENT_NONE;

	short scrollDelta = 0;
	wv::Vector2i position;
};

static std::vector<WindowsMouseEvent> s_mouseEvents;

LRESULT CALLBACK MouseHookProc( int nCode, WPARAM wParam, LPARAM lParam ) {
	if ( nCode >= 0 )
	{
		MOUSEHOOKSTRUCTEX* ptr = reinterpret_cast<MOUSEHOOKSTRUCTEX*>( lParam );
		WindowsMouseEvent event;
		
		switch ( wParam )
		{
		case WM_LBUTTONDOWN: break;
		case WM_RBUTTONDOWN: break;
		case WM_MOUSEWHEEL: 
			event.type = WIN_MOUSE_EVENT_SCROLL;
			event.scrollDelta = HIWORD( ptr->mouseData );
			break;
		case WM_MOUSEMOVE:
			event.type = WIN_MOUSE_EVENT_MOVE;
			event.position = { ptr->pt.x, ptr->pt.y };
			break;
		}

		if ( event.type != WIN_MOUSE_EVENT_NONE )
			s_mouseEvents.push_back( event );
	}
	return CallNextHookEx( NULL, nCode, wParam, lParam );
}

void wv::WindowsMouseDriver::initialize( InputSystem* _inputSystem )
{
	IMouseDriver::initialize( _inputSystem );

	m_hMouseHook = SetWindowsHookEx( WH_MOUSE, MouseHookProc, NULL, GetCurrentThreadId() );
	if ( m_hMouseHook == 0 )
	{
		WV_LOG_ERROR( "Failed to hook mouse event\n" );
		wv::Windows::printLastError();
	}

}

void wv::WindowsMouseDriver::shutdown( InputSystem* _inputSystem )
{
	if( m_hMouseHook != 0 )
		UnhookWindowsHookEx( m_hMouseHook );

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

	m_state.buttonStates[ MOUSE_BUTTON_LEFT ]   = GetAsyncKeyState( lrFlipped ? VK_LBUTTON : VK_RBUTTON ) < 0;
	m_state.buttonStates[ MOUSE_BUTTON_RIGHT ]  = GetAsyncKeyState( lrFlipped ? VK_RBUTTON : VK_LBUTTON ) < 0;
	m_state.buttonStates[ MOUSE_BUTTON_MIDDLE ] = GetAsyncKeyState( VK_MBUTTON ) < 0;
	m_state.buttonStates[ MOUSE_BUTTON_X1 ]     = GetAsyncKeyState( VK_XBUTTON1 ) < 0;
	m_state.buttonStates[ MOUSE_BUTTON_X2 ]     = GetAsyncKeyState( VK_XBUTTON2 ) < 0;
	
	for ( auto event : s_mouseEvents )
	{
		switch ( event.type )
		{
		case WIN_MOUSE_EVENT_SCROLL: 
			m_state.scrollDelta += event.scrollDelta; 
			break;
		case WIN_MOUSE_EVENT_MOVE: 
			m_state.position = event.position; 
			m_state.motion += m_state.position - m_prevState.position;
			break;
		}
	}

	s_mouseEvents.clear();

	sendTriggerEvents( _inputSystem );
	sendValueEvents( _inputSystem );
	sendAxisEvents( _inputSystem );
}
