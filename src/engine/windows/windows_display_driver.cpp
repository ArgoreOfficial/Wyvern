#include "windows_display_driver.h"

#include <wv/application.h>
#include <wv/platform/platform.h>

LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	wv::Application* app = wv::Application::getSingleton();
	
	switch ( uMsg )
	{
	case WM_SIZE:
	{
		int width = LOWORD( lParam );
		int height = HIWORD( lParam );
		app->getDisplayDriver()->resize( { width, height } );
	} break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		app->quit();
		break;
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

void wv::Platform::pollEvents()
{
	Application* app = Application::getSingleton();
	HWND hwnd = app->getDisplayDriver()->winGetHandle();

	MSG msg{};
	while ( PeekMessage( &msg, hwnd, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

bool wv::WindowsDisplayDriver::initializeDisplay( uint16_t _width, uint16_t _height )
{
	m_instance = GetModuleHandle( NULL );

	WNDCLASS wc = { 0 };
	wc.lpszClassName = "Wyvern Engine Class";
	wc.hInstance = m_instance;
	wc.lpfnWndProc = WindowProc;
	
	RegisterClassA( &wc );
	
	m_hwnd = CreateWindowA( wc.lpszClassName, "Wyvern", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, _width, _height, 0, 0, m_instance, 0 );
    
	ShowWindow( m_hwnd, SW_SHOW );

	m_workingAreaSize = { _width, _height };
	return true;
}

void wv::WindowsDisplayDriver::shutdown()
{ 
	DestroyWindow( m_hwnd );
	UnregisterClassA( "Wyvern Engine Class", m_instance );
}

wv::Vector2i wv::WindowsDisplayDriver::getDisplaySize( int _displayIndex )
{
	WV_ASSERT( _displayIndex == 0 );

	int width  = GetSystemMetrics( SM_CXSCREEN );
	int height = GetSystemMetrics( SM_CYSCREEN );

	return { width, height };
}
