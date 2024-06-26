#include "Shader.h"

#include <wv/Device/GraphicsDevice.h>

wv::cShader::~cShader()
{
	m_pGraphicsDevice->destroyShader( this );
}
