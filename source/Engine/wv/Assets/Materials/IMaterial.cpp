#include "IMaterial.h"

#include <wv/Pipeline/Pipeline.h>
#include <wv/Device/GraphicsDevice.h>

void wv::IMaterial::setAsActive( GraphicsDevice* _device )
{
	materialCallback();
	_device->setActivePipeline( m_pipeline );
}
