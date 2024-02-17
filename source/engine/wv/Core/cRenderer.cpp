#include "cRenderer.h"

#include <wv/Manager/cContentManager.h>

#include <cm/backends/cBackend_OpenGL.h>
#include <cm/backends/cBackend_D3D11.h>
#include <cm/Core/cWindow.h>

#include <wv/Core/cApplication.h>
#include <wv/Graphics/cMesh.h>
#include <wv/Graphics/Primitives.h>
#include <wv/Rendering/RenderPass/iRenderPass.h>

#include <wv/Graphics/cShader.h>


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

	cm::cWindow* window = cApplication::getInstance().getWindow();

	m_backend->create( *window );

	m_gbuffer = m_backend->createFramebuffer();
	
	m_backend->addFramebufferTexture(  // position color buffer
		m_gbuffer, "gPosition",
		cm::eTextureFormat::FramebufferFormat_RGBAf, cm::eTextureType::FramebufferType_Color, 
		window->getWidth(), window->getHeight() );
	
	m_backend->addFramebufferTexture(  // normal color buffer
		m_gbuffer, "gNormal",
		cm::eTextureFormat::FramebufferFormat_RGBAf, cm::eTextureType::FramebufferType_Color, 
		window->getWidth(), window->getHeight() );
	
	m_backend->addFramebufferTexture(  // color + specular color buffer
		m_gbuffer, "gAlbedoSpec",
		cm::eTextureFormat::FramebufferFormat_RGBA, cm::eTextureType::FramebufferType_Color, 
		window->getWidth(), window->getHeight() );

	m_backend->attachFramebuffer( m_gbuffer );

	m_backend->addFramebufferRenderbuffer( // depth renderbuffer
		m_gbuffer, 
		cm::eRenderbufferType::RenderbufferType_Depth, 
		window->getWidth(), window->getHeight() );

	m_screen_quad = Primitives::quad( 1.0f );

	cContentManager& content_manager = cContentManager::getInstance();
	// m_gpass_shader     = content_manager.loadShader( "res/shaders/deferred/s_gpass" );
	// m_lightpass_shader = content_manager.loadShader( "res/shaders/deferred/s_lightpass" );
	m_screen_shader = content_manager.loadShader( "res/shaders/deferred/s_screen" );
	
	// TODO: recreate FBO on resize

}

void wv::cRenderer::onResize( int _width, int _height )        { m_backend->onResize( _width, _height ); }
void wv::cRenderer::clear   ( unsigned int _color, int _mode ) { m_backend->clear( _color, ( cm::eClearMode )_mode ); }

void wv::cRenderer::begin( void ) 
{ 
	m_backend->begin(); 
	m_backend->bindFramebuffer( &m_gbuffer );
}

void wv::cRenderer::end( void ) 
{ 
	m_backend->bindFramebuffer( 0 );

	clear( 0xFF0000FF, cm::eClearMode::ClearMode_Color | cm::eClearMode::ClearMode_Depth );
	
	m_backend->useShaderProgram( m_screen_shader->shader_program_handle );

	for ( int i = 0; i < m_gbuffer.textures.size(); i++ )
	{
		int shader_loc = m_backend->getUniformLocation( m_screen_shader->shader_program_handle, m_gbuffer.textures[ i ].name.c_str() );
		m_backend->setUniformInt( shader_loc, i );
		m_backend->setActiveTextureSlot( i );
		m_backend->bindTexture2D( m_gbuffer.textures[ i ].handle );
	}
	
	m_backend->bindVertexArray( m_screen_quad->vertex_array );
	m_backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );

	m_backend->end();	
}

void wv::cRenderer::addRenderPass( iRenderPass* _render_pass )
{
	m_render_passes.push_back( _render_pass );
}
