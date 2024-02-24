#include "cRenderer.h"

#include <wv/Managers/cContentManager.h>

#include <cm/backends/cBackend_OpenGL.h>
#include <cm/backends/cBackend_D3D11.h>
#include <cm/Core/cWindow.h>

#include <wv/Core/cApplication.h>
#include <wv/Graphics/Primitives.h>

#include <wv/Rendering/RenderPass/iRenderPass.h>
#include <wv/Rendering/RenderPass/cDeferredLightRenderPass.h>
#include <wv/Rendering/RenderPass/cAssemblerPass.h>

#include <wv/Rendering/cFramebuffer.h>

wv::cRenderer::cRenderer( void ):
	m_backend{ nullptr }
{

}

wv::cRenderer::~cRenderer( void )
{
	// TODO: move to destroy();

	delete m_gbuffer;
	delete m_backend;
	
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
	m_gbuffer->addTexture( "gbuffer_Position",          cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gbuffer_Normal",            cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gbuffer_Albedo",            cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gbuffer_MetallicRoughness", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gbuffer_Depth",             cm::TextureFormat_RGBAf, cm::TextureType_Depth );
	m_gbuffer->finalize();
	
	m_screen_quad = Primitives::quad( 1.0f );

	m_render_passes.push_back( new cDeferredLightRenderPass() );
	m_assembler = new cAssemblerPass();

	for ( int i = 0; i < m_render_passes.size(); i++ )
		m_render_passes[ i ]->onCreate();

	m_assembler->onCreate();
}

void wv::cRenderer::onDestroy()
{
	for ( int i = 0; i < m_render_passes.size(); i++ )
	{
		m_render_passes[ i ]->onDestroy();
		delete m_render_passes[ i ];
	}
	m_render_passes.clear();

	m_assembler->onDestroy();
	delete m_assembler;

	wv::cRenderer::destroy(); // TODO: clean up other singletons
}

void wv::cRenderer::onResize( int _width, int _height ) 
{ 
	m_backend->onResize( _width, _height );
	m_gbuffer->onResize();

	for ( int i = 0; i < m_render_passes.size(); i++ )
		m_render_passes[ i ]->onResize( _width, _height );
}
void wv::cRenderer::clear   ( unsigned int _color, int _mode ) { m_backend->clear( _color, ( cm::eClearMode )_mode ); }

void wv::cRenderer::begin( void ) 
{ 
	m_backend->begin();
	m_gbuffer->bind();
}

void wv::cRenderer::end( void ) 
{ 
	m_gbuffer->unbind();

	cFramebuffer* input_buffer = m_gbuffer;
	for ( int i = 0; i < m_render_passes.size(); i++ )
	{
		m_render_passes[ i ]->bind();
		
		m_render_passes[ i ]->execute( input_buffer );
		m_backend->bindVertexArray( m_screen_quad->vertex_array );
		m_backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );
		
		m_render_passes[ i ]->unbind();
		
		input_buffer = m_render_passes[ i ]->getFramebuffer();
	}
	
	// final pass
	m_assembler->execute( input_buffer );
	m_backend->bindVertexArray( m_screen_quad->vertex_array );
	m_backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );

	m_backend->end();
}

void wv::cRenderer::addRenderPass( iRenderPass* _render_pass )
{
	m_render_passes.push_back( _render_pass );
}
