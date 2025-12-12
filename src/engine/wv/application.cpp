#include "application.h"

#include <wv/math/math.h>
#include <wv/debug/log.h>
#include <wv/memory/memory.h>

#include <wv/platform/platform.h>
#include <wv/filesystem/file_system.h>

#ifdef WV_SUPPORT_SDL2
#include <sdl/display_driver_sdl.h>
#endif

#include <cmath>
#include <stdio.h>

wv::Application* wv::Application::singleton = nullptr;

void glfwErrorCallback( int error, const char* description )
{
	printf( "Error: %s\n", description );
}

wv::Application::Application()
{
	singleton = this;
}

bool wv::Application::initialize( int _windowWidth, int _windowHeight )
{
	m_graphicsDriverName = "opengl"; // TODO

	m_displayDriver = WV_NEW( DisplayDriverSDL );
	
	if ( !m_displayDriver->initializeDisplay( _windowWidth, _windowHeight ) )
	{
		WV_FREE( m_displayDriver );
		return false;
	}

	if ( !m_renderer.setup() )
		return false;

	m_filesystem = Platform::createFileSystem( "data" );

	///////////////////////////////////////////////////////////////////////////
	// Set up camera

	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	m_camera = new wv::ICamera( wv::ICamera::kOrthographic, windowSize.x, windowSize.y );
	m_camera->getTransform().setPosition( { 0.0f, 0.0f, 0.0f } );
	m_camera->setOrthoWidth( 6.0f );

	///////////////////////////////////////////////////////////////////////////
	// Set up mesh stuff (testing)

	std::string vs = m_filesystem->loadString( "debug_vs.glsl" );
	std::string fs = m_filesystem->loadString( "debug_fs.glsl" );

	m_material = m_renderer.createMaterial();
	m_renderer.setMaterialVertexShader( m_material, vs.c_str() );
	m_renderer.setMaterialFragmentShader( m_material, fs.c_str() );
	m_renderer.finalizeMaterial( m_material );

	std::vector<wv::Vector3f> positions = {
		{  0.0f,  0.5f, 0.5f },
		{  0.5f, -0.5f, 0.5f },
		{ -0.5f, -0.5f, 0.5f }
	};

	std::vector<wv::VertexData> datas = {
		{ {}, { 1.0f, 0.0f, 0.0f } },
		{ {}, { 0.0f, 1.0f, 0.0f } },
		{ {}, { 0.0f, 0.0f, 1.0f } }
	};

	ResourceID mesh = m_renderer.createRenderMesh( positions.data(), positions.size(), datas.data(), sizeof( wv::VertexData ) * datas.size() );
	m_renderer.setRenderMeshMaterial( mesh, m_material );

	m_renderView.renderMeshes.push_back( mesh );
	m_renderView.sceneData.viewProj = wv::Matrix4x4f::identity( 1.0 );
}

bool wv::Application::tick()
{
	if ( !m_alive )
		return false;

	m_displayDriver->swapBuffers();
	m_displayDriver->processEvents();

	// update runtime and deltatime
	
	uint64_t ticks = m_displayDriver->getHighResolutionCounter();
	m_runtime = m_displayDriver->getTicks() / 1000.0;
	m_deltatime = (double)( ( ticks - m_lastTicks ) / (double)m_displayDriver->getHighResolutionFrequency() );

	render();

	m_lastTicks = ticks;
	return true;
}

void wv::Application::shutdown()
{
	m_renderer.destroyMaterial( m_material );

	for ( auto& mesh : m_renderView.renderMeshes )
		m_renderer.destroyRenderMesh( mesh );
	
	m_renderer.shutdown();

	m_displayDriver->shutdown();
}

void wv::Application::update()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();
	
	m_camera->setPixelSize( (size_t)windowSize.x, (size_t)windowSize.y );

	//m_accumulator += m_deltatime;
	//while ( m_accumulator > m_fixed_delta_time )
	//{
	//	m_app->onFixedUpdate( m_fixed_delta_time );
	//
	//	m_fixed_runtime += m_fixed_delta_time;
	//	m_accumulator -= m_fixed_delta_time;
	//}

	//m_app->preUpdate();

	m_camera->update( m_deltatime );
	//m_app->onUpdate( m_deltatime );
	//m_sprite_renderer->update();

	//m_app->postUpdate();
}

void wv::Application::render()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	m_renderer.prepare( windowSize.x, windowSize.y );
	m_renderer.clear( std::sinf( m_runtime * 10.0f ) * 0.5f + 0.5f, 0.0f, 0.0f, 1.0f );

	m_renderer.drawRenderView( m_renderView );
	
	// m_sprite_renderer->drawSprites();

	m_renderer.finalize();
}
