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

	m_displayDriver = Platform::createDisplayDriver();;

	if ( !m_displayDriver->initializeDisplay( _windowWidth, _windowHeight ) )
	{
		WV_FREE( m_displayDriver );
		return false;
	}

	if ( !m_renderer.setup() )
		return false;

	m_filesystem = Platform::createFileSystem( "data" );

	std::string vsDebug = m_filesystem->loadString( "debug_line_vs.glsl" );
	std::string fsDebug = m_filesystem->loadString( "debug_line_fs.glsl" );
	m_renderer.setupDebug( vsDebug.c_str(), fsDebug.c_str() );

	///////////////////////////////////////////////////////////////////////////
	// Set up scene

	Scene* scene = WV_NEW( Scene );

	///////////////////////////////////////////////////////////////////////////
	// Set up camera

	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	ICamera* camera = WV_NEW( ICamera, ICamera::kPerspective, windowSize.x, windowSize.y );
	camera->getTransform().setPosition( { 0.0f, 0.0f, 5.0f } );
	// camera->setOrthoWidth( 6.0f );

	scene->cameras.push_back( camera );

	///////////////////////////////////////////////////////////////////////////
	// Set up mesh stuff (testing)

	std::string vs = m_filesystem->loadString( "debug_vs.glsl" ); // TODO: rename files
	std::string fs = m_filesystem->loadString( "debug_fs.glsl" );

	m_material = m_renderer.createMaterial();
	m_renderer.setMaterialVertexShader( m_material, vs.c_str() );
	m_renderer.setMaterialFragmentShader( m_material, fs.c_str() );
	m_renderer.finalizeMaterial( m_material );

	std::vector<wv::Vector3f> positions = {
		{ -1, -1, -1 },
		{  1, -1, -1 },
		{  1,  1, -1 },
		{ -1,  1, -1 },
		{ -1, -1,  1 },
		{  1, -1,  1 },
		{  1,  1,  1 },
		{ -1,  1,  1 }
	};

	std::vector<uint16_t> indices = {
		0, 1, 3, 3, 1, 2,
		1, 5, 2, 2, 5, 6,
		5, 4, 6, 6, 4, 7,
		4, 0, 7, 7, 0, 3,
		3, 2, 7, 7, 2, 6,
		4, 5, 0, 0, 5, 1
	};

	std::vector<wv::VertexData> datas = {
		{ {}, { 1.0f, 0.0f, 0.0f } },
		{ {}, { 0.0f, 1.0f, 0.0f } },
		{ {}, { 0.0f, 0.0f, 1.0f } },
		{ {}, { 0.0f, 0.0f, 0.0f } },
		{ {}, { 1.0f, 0.0f, 0.0f } },
		{ {}, { 0.0f, 1.0f, 0.0f } },
		{ {}, { 0.0f, 0.0f, 1.0f } },
		{ {}, { 0.0f, 0.0f, 0.0f } },
	};

	ResourceID mesh = m_renderer.createRenderMesh(
		positions.data(), positions.size(),
		indices.data(), indices.size(),
		datas.data(), sizeof( wv::VertexData ) * datas.size() );

	m_renderer.setRenderMeshMaterial( mesh, m_material );

	scene->models.push_back( mesh );
	m_scenes.push_back( scene );
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

	update();
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

	for ( size_t i = 0; i < m_scenes.size(); i++ )
	{
		for ( size_t j = 0; j < m_scenes[ i ]->cameras.size(); j++ )
			WV_FREE( m_scenes[ i ]->cameras[ j ] );
		
		WV_FREE( m_scenes[ i ] );
	}
}

void wv::Application::update()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	Scene* activeScene = getActiveScene();
	if ( !activeScene ) return;

	ICamera* camera = activeScene->getActiveCamera();
	if ( !camera ) return;

	camera->getTransform().setPosition(
		{
			std::cosf( m_runtime ) * 10,
			0,
			std::sinf( m_runtime ) * 10
		} );
	camera->getTransform().setRotation( { 0, wv::Math::degrees( (float)-m_runtime ) + 90, 0 } );

	camera->setPixelSize( (size_t)windowSize.x, (size_t)windowSize.y );

	//m_accumulator += m_deltatime;
	//while ( m_accumulator > m_fixed_delta_time )
	//{
	//	m_app->onFixedUpdate( m_fixed_delta_time );
	//
	//	m_fixed_runtime += m_fixed_delta_time;
	//	m_accumulator -= m_fixed_delta_time;
	//}

	//m_app->preUpdate();

	camera->update( m_deltatime );
	//m_app->onUpdate( m_deltatime );
	//m_sprite_renderer->update();

	//m_app->postUpdate();
}

void wv::Application::render()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();
	m_renderer.prepare( windowSize.x, windowSize.y );
	m_renderer.clearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	m_renderer.clearDepth();

	Scene* activeScene = getActiveScene();
	if ( !activeScene ) return;

	ICamera* camera = activeScene->getActiveCamera();
	if ( !camera ) return;

	m_renderView.sceneData.viewProj = camera->getViewMatrix() * camera->getProjectionMatrix();

	m_renderView.renderMeshes.clear();
	for ( size_t i = 0; i < activeScene->models.size(); i++ )
		m_renderView.renderMeshes.push_back( activeScene->models[ i ] );

	m_renderer.drawRenderView( m_renderView );

	std::vector<Line3f> lines;

	lines.push_back( Line3f{ { 0.f, 0.f, 0.f }, { 1.3f, 1.2f + std::sinf( m_runtime ), 1.7f } });
	lines.push_back( Line3f{      lines.back(), { 2.0f, 1.0f, 3.0f } } );

	m_renderer.clearDepth(); // optional
	m_renderer.drawDebugLines( lines );

	m_renderer.finalize();
}
