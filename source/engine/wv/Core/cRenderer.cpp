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

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


wv::cRenderer::cRenderer( void ):
	m_backend{ nullptr }
{

}

wv::cRenderer::~cRenderer( void )
{
	// TODO: move to destroy();

	delete m_gbuffer;
	delete m_backend;
	delete m_viewbuffer;
	delete m_msbuffer;
}

void wv::cRenderer::create()
{
	if ( m_backend )
		delete m_backend;

	createBackend();
	initImgui();

	cm::cWindow* window = cApplication::getInstance().getWindow();

	m_backend->create( *window );

	createGBuffer( 4 );

	{ // final screen buffer
		m_viewbuffer = new cFramebuffer();
		m_viewbuffer->create();
		m_viewbuffer->addTexture( "viewbuffer_screen", cm::TextureFormat_RGBA, cm::TextureType_Color );
		m_viewbuffer->finalize();
		//m_viewbuffer->addRenderbuffer( "viewbuffer_depth", cm::RenderbufferType_Depth );
	}

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

void wv::cRenderer::createGBuffer( int _samples )
{
	if ( m_gbuffer )
		delete m_gbuffer;

	m_gbuffer = new cFramebuffer();
	m_gbuffer->create( _samples );
	m_gbuffer->addTexture( "gbuffer_Position",          cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gbuffer_Normal",            cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gbuffer_Albedo",            cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gbuffer_MetallicRoughness", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_gbuffer->addTexture( "gbuffer_Depth",             cm::TextureFormat_RGBAf, cm::TextureType_Depth );
	m_gbuffer->finalize();
	
	// multisample intermediate buffer, identical to gbuffer but not multisampled
	m_msbuffer = new cFramebuffer();
	m_msbuffer->create();
	for ( int i = 0; i < m_gbuffer->m_framebuffer_object.textures.size(); i++ )
	{
		cm::sTexture2D& texture = m_gbuffer->m_framebuffer_object.textures[ i ];
		m_msbuffer->addTexture( texture.name, texture.format, texture.type );
	}
	m_msbuffer->finalize();
}

void wv::cRenderer::onResize( int _width, int _height ) 
{
	if ( m_viewport_size.x == _width && m_viewport_size.y == _height )
		return;

	m_next_viewport_size.x = _width;
	m_next_viewport_size.y = _height;
}

void wv::cRenderer::clear( unsigned int _color, int _mode ) { m_backend->clear( _color, ( cm::eClearMode )_mode ); }

float wv::cRenderer::getViewportAspect( void )
{
	if ( m_viewport_size.x == 0 || m_viewport_size.y == 0 )
		return 1.0f;

	return (float)m_viewport_size.x / (float)m_viewport_size.y;
}

void wv::cRenderer::begin( void )
{ 
	onResizeInternal();

	m_backend->begin();
	m_gbuffer->bind();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void wv::cRenderer::end( void ) 
{ 
	m_gbuffer->unbind();

	// blit gbuffer onto intermediate msbuffer
	m_backend->blitFramebuffer( m_gbuffer->m_framebuffer_object, m_msbuffer->m_framebuffer_object );

	// execute render passes
	
	cFramebuffer* input_buffer = m_msbuffer;
	for ( int i = 0; i < m_render_passes.size(); i++ )
	{
		m_render_passes[ i ]->bind();
		
		m_render_passes[ i ]->execute( input_buffer );
		m_backend->bindVertexArray( m_screen_quad->vertex_array );
		m_backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );
		
		m_render_passes[ i ]->unbind();
		
		input_buffer = m_render_passes[ i ]->getFramebuffer();
	}
}

void wv::cRenderer::display( void )
{
	ImGui::Render();
#if defined( WV_DEBUG )
	m_viewbuffer->bind();
#endif
	
	m_assembler->execute( m_render_passes.back()->getFramebuffer() );
	m_backend->bindVertexArray( m_screen_quad->vertex_array );
	m_backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );

#if defined( WV_DEBUG )
	m_viewbuffer->unbind();
#endif
	
	m_backend->end();

	// imgui
	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent( backup_current_context );
	}
}


void wv::cRenderer::addRenderPass( iRenderPass* _render_pass )
{
	m_render_passes.push_back( _render_pass );
}

void wv::cRenderer::createBackend()
{
	switch ( m_backend_type )
	{
	case BackendType_OpenGL:     m_backend = new cm::cBackend_OpenGL(); break;
	case BackendType_Direct3D11: m_backend = new cm::cBackend_D3D11(); break;

	default: m_backend = new cm::cBackend_OpenGL();
	}
}

void wv::cRenderer::initImgui()
{
	IMGUI_CHECKVERSION();
	
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	
	switch ( m_backend_type )
	{
	case BackendType_OpenGL:
		ImGui_ImplGlfw_InitForOpenGL( cApplication::getInstance().getWindow()->getWindowObject(), true );
		ImGui_ImplOpenGL3_Init( "#version 460" );
		break;
	case BackendType_Direct3D11:
		printf( "Imgui not implemented for D3D11 yet" );
		break;
	}

	
}

void wv::cRenderer::deinitImgui( void )
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


void wv::cRenderer::onResizeInternal()
{
	m_viewport_size = m_next_viewport_size;
	int width  = m_viewport_size.x;
	int height = m_viewport_size.y;

	m_backend   ->onResize( width, height );
	m_gbuffer   ->onResize( width, height );
	m_viewbuffer->onResize( width, height );
	m_msbuffer->onResize( width, height );

	for ( int i = 0; i < m_render_passes.size(); i++ )
		m_render_passes[ i ]->onResize( width, height );
}
