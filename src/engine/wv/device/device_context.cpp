#include "device_context.h"

#include <noapi/noapi_device_context.h>

#ifdef WV_SUPPORT_GLFW
#include <glfw/glfw_device_context.h>
#endif

#ifdef WV_SUPPORT_SDL2
#include <sdl/sdl_device_context.h>
#endif

#ifdef WV_PLATFORM_PSVITA
#include <psp2/psp2_device_context.h>
#endif

#include <wv/memory/memory.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::iDeviceContext* wv::iDeviceContext::getDeviceContext( ContextDesc* _desc )
{
	iDeviceContext* context = nullptr;

	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Device Context\n" );

	switch ( _desc->deviceApi )
	{

	case WV_DEVICE_CONTEXT_API_NONE:
		context = WV_NEW( NoAPIDeviceContext );
		break;

	#ifdef WV_SUPPORT_GLFW
	case WV_DEVICE_CONTEXT_API_GLFW: 
		context = WV_NEW( GLFWDeviceContext ); 
		break;
	#endif

	#ifdef WV_SUPPORT_SDL2
	case WV_DEVICE_CONTEXT_API_SDL: 
		context = WV_NEW( SDLDeviceContext ); 
		break;
	#endif

	#ifdef WV_PLATFORM_PSVITA
	case WV_DEVICE_CONTEXT_API_PSVITA: 
		context = WV_NEW( cPSVitaDeviceContext );
		break;
	#else

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

