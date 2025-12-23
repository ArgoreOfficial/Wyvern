#include "application.h"

#include <wv/math/math.h>
#include <wv/debug/log.h>
#include <wv/display_driver.h>
#include <wv/filesystem/file_system.h>
#include <wv/memory/memory.h>
#include <wv/platform/platform.h>
#include <wv/reflection/reflection.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/entity/world_sector.h>

#include <wv/graphics/systems/render_world_system.h>
#include <wv/graphics/components/mesh_component.h>
#include <wv/graphics/viewport.h>

#include <wv/camera/components/orbit_camera_component.h>
#include <wv/camera/systems/camera_manager_system.h>

#include <cmath>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::Application* wv::Application::singleton = nullptr;

///////////////////////////////////////////////////////////////////////////////////////

void glfwErrorCallback( int error, const char* description )
{
	printf( "Error: %s\n", description );

}

///////////////////////////////////////////////////////////////////////////////////////

wv::Application::Application()
{
	singleton = this;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Application::initialize( World* _world, int _windowWidth, int _windowHeight )
{
	m_graphicsDriverName = "opengl"; // TODO

	m_displayDriver = Platform::createDisplayDriver();

	if ( !m_displayDriver->initializeDisplay( _windowWidth, _windowHeight ) )
	{
		WV_FREE( m_displayDriver );
		return false;
	}

	if ( !m_renderer.setup() )
		return false;

	m_world = _world;

	m_filesystem = Platform::createFileSystem( "data" );

	std::string vsDebug = m_filesystem->loadString( "debug_line_vs.glsl" );
	std::string fsDebug = m_filesystem->loadString( "debug_line_fs.glsl" );
	m_renderer.setupDebug( vsDebug.c_str(), fsDebug.c_str() );

	///////////////////////////////////////////////////////////////////////////
	// Set up camera

	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	Viewport* viewport = m_world->getViewport();
	if ( !viewport )
	{
		viewport = WV_NEW( Viewport );
		m_world->setViewport( viewport );
	}
	
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

	///////////////////////////////////////////////////////////////////////////
	// Set up world

	m_world->createWorldSystem<RenderWorldSystem>();
	m_world->createWorldSystem<CameraManagerSystem>();

	MeshComponent* meshComponent = WV_NEW( MeshComponent );
	meshComponent->setRenderMesh( mesh );

	Entity* entity = WV_NEW( Entity );
	entity->addComponent( meshComponent );

	OrbitCameraComponent* cameraComponent = WV_NEW( OrbitCameraComponent );
	Entity* cameraEntity = WV_NEW( Entity );
	cameraEntity->getTransform().setPosition( { 0.0f, 0.0f, 5.0f } );
	cameraEntity->addComponent( cameraComponent );

	WorldSector* sector = WV_NEW( WorldSector );
	sector->addEntity( entity );
	sector->addEntity( cameraEntity );
	m_world->addSector( sector );

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::shutdown()
{
	m_world->shutdown();
	WV_FREE( m_world );

	m_renderer.destroyMaterial( m_material );

	m_renderer.shutdown();
	m_displayDriver->shutdown();

	ReflectionRegistry::destroySingleton();
}

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::update()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	m_world->updateLoading();
	m_world->updateSectors( m_deltatime );

	//ICamera* camera = m_world->activeCamera;
	//if ( !camera ) return;
	//
	//camera->getTransform().setPosition(
	//	{
	//		std::cosf( m_runtime ) * 10,
	//		0,
	//		std::sinf( m_runtime ) * 10
	//	} );
	//camera->getTransform().setRotation( { 0, wv::Math::degrees( (float)-m_runtime ) + 90, 0 } );
	//
	//camera->setPixelSize( (size_t)windowSize.x, (size_t)windowSize.y );

	CameraManagerSystem* cameraManagerSystem = m_world->getWorldSystem<CameraManagerSystem>();
	if ( cameraManagerSystem->hasActiveCamera() )
	{
		m_world->getViewport()->setSize( windowSize.x, windowSize.y );
		m_world->getViewport()->setCamera( cameraManagerSystem->getActiveCamera()->getUnderlyingCamera() );
	}

	//m_accumulator += m_deltatime;
	//while ( m_accumulator > m_fixed_delta_time )
	//{
	//	m_app->onFixedUpdate( m_fixed_delta_time );
	//
	//	m_fixed_runtime += m_fixed_delta_time;
	//	m_accumulator -= m_fixed_delta_time;
	//}

	//m_app->preUpdate();

	//m_app->onUpdate( m_deltatime );
	//m_sprite_renderer->update();

	//m_app->postUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::render()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();
	m_renderer.prepare( windowSize.x, windowSize.y );
	m_renderer.clearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	m_renderer.clearDepth();
	m_renderer.renderWorld( m_world );

	std::vector<Line3f> lines;

	lines.push_back( Line3f{ { 0.f, 0.f, 0.f }, { 1.3f, 1.2f + std::sinf( m_runtime ), 1.7f } } );
	lines.push_back( Line3f{ lines.back(), { 2.0f, 1.0f, 3.0f } } );

	m_renderer.clearDepth(); // optional
	m_renderer.drawDebugLines( lines );

	m_renderer.finalize();
}
