#include "DeviceContext.h"

#include <wv/Device/DeviceContext/GLFW/GLFWDeviceContext.h>
#include <wv/Device/DeviceContext/SDL/SDLDeviceContext.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::iDeviceContext* wv::iDeviceContext::getDeviceContext( ContextDesc* _desc )
{
	iDeviceContext* context = nullptr;

	switch ( _desc->deviceApi )
	{
	case WV_DEVICE_CONTEXT_API_GLFW: context = new GLFWDeviceContext(); break;
	case WV_DEVICE_CONTEXT_API_SDL:  context = new SDLDeviceContext (); break;
	}

	if ( context )
	{
		if ( !context->initialize( _desc ) )
		{
			delete context;
			return nullptr;
		}

		context->m_deviceApi = _desc->deviceApi;
		context->m_graphicsApi = _desc->graphicsApi;
		context->m_graphicsApiVersion = _desc->graphicsApiVersion;
	}

	return context;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iDeviceContext::setSize( int _width, int _height )
{
	m_width = _width;
	m_height = _height;
}

