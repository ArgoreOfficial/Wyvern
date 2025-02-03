#include "DeviceContext.h"

#include <wv/Device/DeviceContext/GLFW/GLFWDeviceContext.h>
#include <wv/Device/DeviceContext/SDL/SDLDeviceContext.h>

#ifdef WV_PLATFORM_PSVITA
#include <wv/Device/DeviceContext/PSVita/PSVitaDeviceContext.h>
#endif

#include <wv/Memory/Memory.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::iDeviceContext* wv::iDeviceContext::getDeviceContext( ContextDesc* _desc )
{
	iDeviceContext* context = nullptr;

	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Device Context\n" );

#ifdef WV_PLATFORM_PSVITA
	context = new cPSVitaDeviceContext(); // force psvita context
#else // WV_PLATFORM_PSVITA
	switch ( _desc->deviceApi )
	{
	case WV_DEVICE_CONTEXT_API_NONE:
		wv::Debug::Print( Debug::WV_PRINT_WARN, "_desc->deviceApi was WV_DEVICE_CONTEXT_API_NONE\n" );
		context = nullptr;
		break;

	#ifdef WV_SUPPORT_GLFW
	case WV_DEVICE_CONTEXT_API_GLFW: context = new GLFWDeviceContext(); break;
	#endif

	#ifdef WV_SUPPORT_SDL2
	case WV_DEVICE_CONTEXT_API_SDL:  context = new SDLDeviceContext(); break;
	#endif
	}
#endif // WV_PLATFORM_PSVITA

	if( !context )
		return nullptr;

	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Initializing Device Context\n" );
	if ( !context->initialize( _desc ) )
	{
		WV_FREE( context );
		return nullptr;
	}

	context->m_deviceApi          = _desc->deviceApi;
	context->m_graphicsApi        = _desc->graphicsApi;
	context->m_graphicsApiVersion = _desc->graphicsApiVersion;

	context->setSize( _desc->width, _desc->height );	

	return context;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iDeviceContext::setSize( int _width, int _height )
{
	m_width  = _width;
	m_height = _height;
}

