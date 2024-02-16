#include "cRenderer.h"

#include <cm/backends/cBackend_OpenGL.h>
#include <cm/backends/cBackend_D3D11.h>

#include <wv/Core/cApplication.h>
#include <wv/Rendering/RenderPass/iRenderPass.h>

wv::cRenderer::cRenderer( void ):
	m_backend{ nullptr }
{

}

wv::cRenderer::~cRenderer( void )
{

}

void wv::cRenderer::create()
{
	if ( m_backend )
		delete m_backend;

	switch ( m_backend_type )
	{
	case BackendType_OpenGL:     m_backend = new cm::cBackend_OpenGL(); break;
	case BackendType_Direct3D11: m_backend = new cm::cBackend_D3D11(); break;

	default: m_backend = new cm::cBackend_OpenGL();
	}

	m_backend->create( *cApplication::getInstance().getWindow() );

	createDefaultShader();
}

void wv::cRenderer::onResize( int _width, int _height ) { m_backend->onResize( _width, _height ); }
void wv::cRenderer::clear   ( unsigned int _color )     { m_backend->clear   ( _color ); }

void wv::cRenderer::begin( void ) { m_backend->begin(); }
void wv::cRenderer::end  ( void ) { m_backend->end  (); }

void wv::cRenderer::addRenderPass( iRenderPass* _render_pass )
{
	m_render_passes.push_back( _render_pass );
}

void wv::cRenderer::createDefaultShader( void )
{
	cm::Shader::sShader vertex_shader   = m_backend->createShader( m_shader_default_vert, cm::Shader::eShaderType::ShaderType_Vertex );
	cm::Shader::sShader fragment_shader = m_backend->createShader( m_shader_default_frag, cm::Shader::eShaderType::ShaderType_Fragment );
	
	m_shader_default = m_backend->createShaderProgram();
	
	m_backend->attachShader( m_shader_default, vertex_shader );
	m_backend->attachShader( m_shader_default, fragment_shader );

	m_backend->linkShaderProgram( m_shader_default );
}
