#include "cShader.h"

#include <cm/Backends/iBackend.h>
#include <wv/Core/cRenderer.h>

wv::cShader::cShader()
{
}

wv::cShader::~cShader()
{
}

int wv::cShader::getUniformLocation( std::string _uniform )
{
	if ( m_uniforms.count( _uniform ) == 0 )
	{
		cm::Shader::sShaderUniform uniform;
		uniform.location = cRenderer::getInstance().getBackend()->getUniformLocation( shader_program_handle, _uniform.c_str() );
		m_uniforms[ _uniform ] = uniform;
	}

	return m_uniforms[ _uniform ].location;
}

void wv::cShader::setMatrix( std::string _uniform, float* _matrix_ptr )
{
	int location = getUniformLocation( _uniform );
	cRenderer::getInstance().getBackend()->setUniformMat4f( location, _matrix_ptr );
}

void wv::cShader::setVec4f( std::string _uniform, const wv::cVector4f& _vec )
{
	int location = getUniformLocation( _uniform );
	cRenderer::getInstance().getBackend()->setUniformVec4f( location, _vec );
}
