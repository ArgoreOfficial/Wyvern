#include "NoAPIDeviceContext.h"

#include <wv/Debug/Print.h>

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
	auto now = m_timer.now();
	
	if( m_isFirstFrame )
		m_isFirstFrame = false;
	else
	{
		using cast_t = std::chrono::duration<double>;
		double deltaTime = std::chrono::duration_cast<cast_t>( now - m_timepoint ).count();

		m_deltaTime = deltaTime;
		m_time += deltaTime;
	}

	m_timepoint = now;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::onResize( int _width, int _height )
{
	iDeviceContext::onResize( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::setSize( int _width, int _height )
{
	iDeviceContext::setSize( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::setMouseLock( bool _lock )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIDeviceContext::setTitle( const char* _title )
{

}
