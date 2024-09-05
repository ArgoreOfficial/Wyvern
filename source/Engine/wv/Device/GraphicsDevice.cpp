#include "GraphicsDevice.h"

#include <wv/Debug/Print.h>
#include <wv/Device/DeviceContext.h>

#ifdef WV_SUPPORT_GLAD
#include <wv/Device/GraphicsDevice/OpenGLGraphicsDevice.h>
#endif

#ifdef WV_PLATFORM_PSVITA
#include <wv/Device/GraphicsDevice/PSVitaGraphicsDevice.h>
#endif

wv::iGraphicsDevice* wv::iGraphicsDevice::createGraphicsDevice( GraphicsDeviceDesc* _desc )
{
	
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Graphics Device\n" );

	iGraphicsDevice* device = nullptr;
#ifdef WV_PLATFORM_PSVITA
	device = new cPSVitaGraphicsDevice();
#else
#ifdef WV_SUPPORT_GLAD
	device = new cOpenGLGraphicsDevice();
#endif
#endif

	if( !device )
		return nullptr;

	if ( !device->initialize( _desc ) )
	{
		delete device;
		return nullptr;
	}

	_desc->pContext->m_graphicsApiVersion = device->m_graphicsApiVersion;

	return device;
}
