#include "IMaterial.h"

#include <wv/Pipeline/Pipeline.h>
#include <wv/Device/GraphicsDevice.h>

void wv::IMaterial::setAsActive( GraphicsDevice* _device )
{
	_device->setActivePipeline( m_pipeline );
	materialCallback();
}
