#include "cShader.h"

#include <cm/Backends/iBackend.h>
#include <wv/Core/cRenderer.h>
#include <wv/Managers/cContentManager.h>

wv::cShader::cShader()
{
}

wv::cShader::~cShader()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	backend->destroyBuffer( m_uniform_buffer );
	backend->destroyShaderProgram( shader_program_handle );

	delete[] m_uniform_buffer_data;
}

void wv::cShader::createUniformBlock()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	// TODO: remove?
	int loc = -1;
	int uniform_index = 0;
	do
	{
		cm::Shader::sUniform uniform = backend->getUniform( shader_program_handle, uniform_index );
		loc = uniform.location;
		uniform_index++;

		if ( loc == -1 )
			continue;

		m_uniforms[ uniform.name ] = uniform;
	} while ( loc != -1 );


	m_uniform_block = backend->getUniformBlock( shader_program_handle, 0 );
	if ( m_uniform_block.name == "" )
		return;

	m_buffer_slot = cContentManager::getInstance().getUniformBlockLocation();

	m_uniform_buffer = backend->createBuffer( cm::BufferType_Uniform, cm::BufferUsage_Dynamic );
	backend->setUniformBlockBinding( shader_program_handle, m_uniform_block.name.c_str(), m_buffer_slot );
	backend->bindBufferBase( m_uniform_buffer, m_buffer_slot );

	m_uniform_buffer_data = new unsigned char[ m_uniform_block.size ];
}

void wv::cShader::uniformBlockBegin()
{
	
}

void wv::cShader::uniformBlockEnd()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
;
	backend->bindBuffer( m_uniform_buffer );
	backend->bufferData( m_uniform_buffer, m_uniform_buffer_data, m_uniform_block.size );
}

void wv::cShader::uniformBlockBuffer( const std::string& _uniform, void* _data, size_t _size )
{
	if ( m_uniform_block.uniforms.count( _uniform ) == 0 )
		return;

	int offset = m_uniform_block.uniforms[ _uniform ].offset;
	memcpy( &m_uniform_buffer_data[ offset ], _data, _size );
}

int wv::cShader::getUniformLocation( std::string _uniform )
{
	if ( m_uniforms.count( _uniform ) == 0 )
	{
		cm::Shader::sUniform uniform;
		uniform.location = cRenderer::getInstance().getBackend()->getUniformLocation( shader_program_handle, _uniform.c_str() );
		m_uniforms[ _uniform ] = uniform;
	}

	return m_uniforms[ _uniform ].location;
}