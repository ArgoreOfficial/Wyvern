#ifdef WV_PLATFORM_WINDOWS

#pragma once

#include <wv/display_driver.h>

#include <Windows.h>

namespace wv {

class WindowsDisplayDriver : public wv::DisplayDriver
{
public:
	WindowsDisplayDriver() { }

	virtual bool initializeDisplay( uint16_t _width, uint16_t _height ) override;
	virtual void shutdown() override;

	virtual wv::Vector2i getDisplaySize( int _displayIndex = 0 ) override;

	bool isMinimized() const override { return IsIconic( m_hwnd ); }

	virtual HWND winGetHandle() override { return m_hwnd; }
	HINSTANCE getInstanceHandle() const { return m_instance; }

private:
	HWND m_hwnd = 0;
	HINSTANCE m_instance = 0;
};

}

#endif