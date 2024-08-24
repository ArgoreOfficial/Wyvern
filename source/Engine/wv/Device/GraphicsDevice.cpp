#include "GraphicsDevice.h"

#include <wv/Device/DeviceContext.h>

#include <wv/Device/GraphicsDevice/OpenGLGraphicsDevice.h>


wv::iGraphicsDevice* wv::iGraphicsDevice::createGraphicsDevice( GraphicsDeviceDesc* _desc )
{
	/// TODO: different backends
	iGraphicsDevice* device = new cOpenGLGraphicsDevice();
	
	if ( !device->initialize( _desc ) )
	{
		delete device;
		return nullptr;
	}
	
	_desc->pContext->m_graphicsApiVersion = device->m_graphicsApiVersion;

	return device;
}
