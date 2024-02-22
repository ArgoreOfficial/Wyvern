#include "cShader.h"

#include <cm/Backends/iBackend.h>
#include <wv/Core/cRenderer.h>
#include <wv/Managers/cContentManager.h>

#include <stdexcept>
#include <format>

wv::cShader::cShader( std::string _name, std::string _path ):
	name{ _name },
	path{ _path }
{
	
}

wv::cShader::~cShader()
{
	destroy();
}

void wv::cShader::destroy()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	
	if ( m_uniform_buffer.handle != 0 )
	{
		backend->destroyBuffer( m_uniform_buffer );
		m_uniform_buffer.handle = 0;
	}
	
	if ( shader_program_handle != 0 )
	{
		backend->destroyShaderProgram( shader_program_handle );
		shader_program_handle = 0;
	}
	
	if( m_uniforms.size() != 0 )
		m_uniforms.clear();

	if ( m_uniform_buffer_data )
	{
		delete[] m_uniform_buffer_data;
		m_uniform_buffer_data = nullptr;
	}
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
	backend->bufferData( m_uniform_buffer, nullptr, m_uniform_block.size );

	m_uniform_buffer_data = new unsigned char[ m_uniform_block.size ];
}

void wv::cShader::ubBegin()
{
	for ( auto& uniform : m_uniform_block.uniforms )
		m_used_uniforms[ uniform.first ] = false;
}

void wv::cShader::ubEnd()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	if ( m_uniform_buffer.handle == 0 )
		return;

	backend->bindBuffer( m_uniform_buffer );
	backend->bufferData( m_uniform_buffer, m_uniform_buffer_data, m_uniform_block.size );

	for ( auto& uniform : m_used_uniforms )
		if ( uniform.second == false )
			printf( "Uniform '%s' was never set. This may cause crashes\n", uniform.first.c_str() );

}

void wv::cShader::ubBufferData( const std::string& _uniform, void* _data, size_t _size )
{
	if ( m_uniform_buffer.handle == 0 )
		return;

	if ( m_uniform_block.uniforms.count( _uniform ) == 0 )
	{
		printf( "[ubError] Unknown uniform '%s'\n", _uniform.c_str() );
		return;
	}

	int offset = m_uniform_block.uniforms[ _uniform ].offset;
	memcpy( &m_uniform_buffer_data[ offset ], _data, _size );
	m_used_uniforms[ _uniform ] = true;
}

int wv::cShader::getUniformLocation( std::string _uniform )
{
	if ( m_uniform_buffer.handle == 0 )
		return 0;

	if ( m_uniforms.count( _uniform ) == 0 )
	{
		cm::Shader::sUniform uniform;
		uniform.location = cRenderer::getInstance().getBackend()->getUniformLocation( shader_program_handle, _uniform.c_str() );
		m_uniforms[ _uniform ] = uniform;
	}

	return m_uniforms[ _uniform ].location;
}
