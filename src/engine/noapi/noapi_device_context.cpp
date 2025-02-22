#include "noapi_device_context.h"

#include <wv/debug/log.h>

///////////////////////////////////////////////////////////////////////////////////////

bool wv::NoAPIDeviceContext::initialize( ContextDesc* /*_desc*/ )
{
	Debug::Print( Debug::WV_PRINT_INFO, "Initialized Device Context\n" );
	Debug::Print( Debug::WV_PRINT_INFO, "  No API\n" );

	return true;
}

void wv::NoAPIDeviceContext::setSwapInterval( int /*_interval*/ )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::terminate()
{

}

void wv::NoAPIDeviceContext::initImGui()
{

}

void wv::NoAPIDeviceContext::terminateImGui()
{

}

bool wv::NoAPIDeviceContext::newImGuiFrame()
{
	return false;
}

void wv::NoAPIDeviceContext::renderImGui()
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::GraphicsDriverLoadProc wv::NoAPIDeviceContext::getLoadProc()
{
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::pollEvents()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::swapBuffers()
{
#ifdef WV_PLATFORM_WINDOWS
	auto now = m_timer.now();
#endif

	if( m_isFirstFrame )
		m_isFirstFrame = false;
	else
	{
	#ifdef WV_PLATFORM_WINDOWS
		using cast_t = std::chrono::duration<double>;
		double deltaTime = std::chrono::duration_cast<cast_t>( now - m_timepoint ).count();
	#else
		double deltaTime = 1 / 60.0;
		std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
	#endif

		m_deltaTime = deltaTime;
		m_time += deltaTime;
	}
#ifdef WV_PLATFORM_WINDOWS
	m_timepoint = now;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::onResize( int _width, int _height )
{
	IDeviceContext::onResize( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::setSize( int _width, int _height )
{
	IDeviceContext::setSize( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::setMouseLock( bool /*_lock*/ )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::setTitle( const char* /*_title*/ )
{

}
