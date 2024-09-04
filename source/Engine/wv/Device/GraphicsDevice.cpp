#include "GraphicsDevice.h"

#include <wv/Device/DeviceContext.h>

#ifdef WV_SUPPORT_GLAD
#include <wv/Device/GraphicsDevice/OpenGLGraphicsDevice.h>
#endif

wv::iGraphicsDevice* wv::iGraphicsDevice::createGraphicsDevice( GraphicsDeviceDesc* _desc )
{
	/// TODO: different backends
	iGraphicsDevice* device = nullptr;
#ifdef WV_SUPPORT_GLAD
	iGraphicsDevice* device = new cOpenGLGraphicsDevice();
#endif

	if( !device )
		return nullptr;

	if ( !device->initialize( _desc ) )
	{
		delete device;
		return nullptr;
	}
	/*
	_desc->pContext->m_graphicsApiVersion = device->m_graphicsApiVersion;
	*/

	return device;
}
