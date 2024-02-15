#include "cShader.h"

#include <cm/Backends/iBackend.h>
#include <wv/Core/cRenderer.h>
#include <wv/Manager/cContentManager.h>

wv::cShader::cShader()
{
}

wv::cShader::~cShader()
{
}

void wv::cShader::createUniformBlock()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	cm::Shader::sShaderUniformBlock block = backend->getUniformBlock( shader_program_handle, 0 );
	m_ubo_size = block.size;

	// TODO: remove
	int loc = -1;
	int uniform_index = 0;
	do
	{
		cm::Shader::sShaderUniform uniform = backend->getUniform( shader_program_handle, uniform_index );
		loc = uniform.location;
		uniform_index++;

		if ( loc == -1 )
			continue;

		m_uniforms[ uniform.name ] = uniform;
	} while ( loc != -1 );

	m_buffer_slot = cContentManager::getInstance().getUniformBlockLocation();

	m_uniform_buffer = backend->createBuffer( cm::BufferType_Uniform, cm::BufferUsage_Dynamic );
	backend->setUniformBlockBinding( shader_program_handle, block.name.c_str(), m_buffer_slot );
	backend->bindBufferBase( m_uniform_buffer, m_buffer_slot );

	m_uniform_buffer_data = new unsigned char[ m_ubo_size ];
}

void wv::cShader::uniformBlockBegin()
{
	m_buffer_offset = 0;
}

void wv::cShader::uniformBlockEnd()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	m_buffer_offset = 0;
	backend->bindBuffer( m_uniform_buffer );
	backend->bufferData( m_uniform_buffer, m_uniform_buffer_data, m_ubo_size );
}

int wv::cShader::getUniformLocation( std::string _uniform )
{
	if ( m_uniforms.count( _uniform ) == 0 )
	{
		return -1;
		/*
		cm::Shader::sShaderUniform uniform;
		uniform.location = cRenderer::getInstance().getBackend()->getUniformLocation( shader_program_handle, _uniform.c_str() );
		m_uniforms[ _uniform ] = uniform;
		*/
	}

	return m_uniforms[ _uniform ].location;
}