#include "cRenderer.h"

#include <wv/Manager/cContentManager.h>

#include <cm/backends/cBackend_OpenGL.h>
#include <cm/backends/cBackend_D3D11.h>
#include <cm/Core/cWindow.h>

#include <wv/Core/cApplication.h>
#include <wv/Graphics/cMesh.h>
#include <wv/Graphics/Primitives.h>

#include <wv/Rendering/RenderPass/iRenderPass.h>
#include <wv/Rendering/cFramebuffer.h>

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




	m_gbuffer = new cFramebuffer();
	m_gbuffer->create();
	m_gbuffer->addTexture( "gPosition",   cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gNormal",     cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gAlbedo", cm::TextureFormat_RGBA,  cm::TextureType_Color );
	m_gbuffer->finalize();
	
	m_2buffer = new cFramebuffer();
	m_2buffer->create();
	m_2buffer->addTexture( "gGamer", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_2buffer->finalize();
	
	m_screen_quad = Primitives::quad( 1.0f );


	cContentManager& content_manager = cContentManager::getInstance();
	// m_gpass_shader     = content_manager.loadShader( "res/shaders/deferred/s_gpass" );
	// m_lightpass_shader = content_manager.loadShader( "res/shaders/deferred/s_lightpass" );


	m_screen_shader  = content_manager.loadShader( "res/shaders/deferred/s_screen" );
	m_screen_shader2 = content_manager.loadShader( "res/shaders/deferred/s_screen_2" );
	m_2pass = content_manager.loadShader( "res/shaders/deferred/s_2pass" );
	
	// TODO: split pass shaders and framebuffer into cRenderPass
}

void wv::cRenderer::onResize( int _width, int _height )        { m_backend->onResize( _width, _height ); }
void wv::cRenderer::clear   ( unsigned int _color, int _mode ) { m_backend->clear( _color, ( cm::eClearMode )_mode ); }

void wv::cRenderer::begin( void ) 
{ 
	m_backend->begin();
	m_gbuffer->bind();
}

void wv::cRenderer::end( void ) 
{ 
	m_gbuffer->unbind();

	// 2pass
	
	m_2buffer->bind();
	
	clear( 0x000000FF, cm::ClearMode_Color | cm::ClearMode_Depth );

	m_backend->useShaderProgram( m_2pass->shader_program_handle );
	m_gbuffer->bindTextures( m_2pass );
	m_backend->bindVertexArray( m_screen_quad->vertex_array );
	m_backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );
	m_2buffer->unbind();
	
	clear( 0x000000FF, cm::ClearMode_Depth );

	// screen pass
	m_backend->useShaderProgram( m_screen_shader->shader_program_handle );
	m_gbuffer->bindTextures( m_screen_shader );
	m_backend->bindVertexArray( m_screen_quad->vertex_array );
	m_backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );
	
	clear( 0x000000FF, cm::ClearMode_Depth );

	// screen2 pass composite
	m_backend->useShaderProgram( m_screen_shader2->shader_program_handle );
	m_2buffer->bindTextures( m_screen_shader2 );
	m_backend->bindVertexArray( m_screen_quad->vertex_array );
	m_backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );

	m_backend->end();
}

void wv::cRenderer::addRenderPass( iRenderPass* _render_pass )
{
	m_render_passes.push_back( _render_pass );
}
