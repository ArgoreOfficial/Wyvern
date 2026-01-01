#include "application.h"


#include <wv/camera/components/orbit_camera_component.h>
#include <wv/camera/systems/camera_manager_system.h>
#include <wv/display_driver.h>
#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/entity/world_sector.h>
#include <wv/event/event_manager.h>
#include <wv/filesystem/file_system.h>
#include <wv/graphics/systems/render_world_system.h>
#include <wv/graphics/components/mesh_component.h>
#include <wv/graphics/viewport.h>
#include <wv/reflection/reflection.h>
#include <wv/input/input_system.h>
#include <wv/math/math.h>
#include <wv/memory/memory.h>
#include <wv/platform/platform.h>


// TODO: MOVE TO WINDOWS DRIVER PLACE SOMEWHERE
#include <windows/xinput_controller_driver.h>
#include <windows/windows_keyboard_driver.h>

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

	// IEngineSystem ?
	// might help with cleanup and such
	// systemManager->createSystem<DisplayDriver>( Platform::createDisplayDriver() );
	// systemManager->createSystem<Renderer>( Platform::createRenderer() );
	// systemManager->createSystem<FileSystem>( Platform::createFileSystem( "data" ) );
	// systemManager->createSystem<InputSystem>();
	// systemManager->initialize();

	m_eventManager = WV_NEW( EventManager );
	m_inputSystem  = WV_NEW( InputSystem );

	m_inputSystem->createInputDriver<XInputControllerDriver>();
	m_inputSystem->createInputDriver<WindowsKeyboardDriver>();

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
	
	ActionGroup* playerActionGroup = m_inputSystem->createActionGroup( "Player" );

	playerActionGroup->bindTriggerAction( "Jump", "Keyboard",   SCANCODE_SPACE );
	playerActionGroup->bindTriggerAction( "Jump", "Controller", CONTROLLER_BUTTON_A );
	
	playerActionGroup->bindAxisAction( "Look", "Controller", AXIS_DIRECTION_ALL, CONTROLLER_JOYSTICK_RIGHT );
	playerActionGroup->bindAxisAction( "Look", "Keyboard", AXIS_DIRECTION_NORTH, SCANCODE_ARROW_UP );
	playerActionGroup->bindAxisAction( "Look", "Keyboard", AXIS_DIRECTION_SOUTH, SCANCODE_ARROW_DOWN );
	playerActionGroup->bindAxisAction( "Look", "Keyboard", AXIS_DIRECTION_EAST, SCANCODE_ARROW_RIGHT );
	playerActionGroup->bindAxisAction( "Look", "Keyboard", AXIS_DIRECTION_WEST, SCANCODE_ARROW_LEFT );
	
	playerActionGroup->bindAxisAction( "Move", "Controller", AXIS_DIRECTION_ALL, CONTROLLER_JOYSTICK_LEFT );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", AXIS_DIRECTION_NORTH, SCANCODE_W );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", AXIS_DIRECTION_SOUTH, SCANCODE_S );
	
	playerActionGroup->bindValueAction( "Throttle", "Keyboard", SCANCODE_E );
	playerActionGroup->bindValueAction( "Throttle", "Controller", CONTROLLER_TRIGGER_LEFT );
	
	playerActionGroup->enable();

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

	OrbitCameraComponent* cameraComponent = WV_NEW( OrbitCameraComponent );
	Entity* cameraEntity = WV_NEW( Entity );
	cameraEntity->getTransform().setPosition( { -1, 10.0f, 10.0f } );
	cameraEntity->getTransform().setRotation( { -45.0f, -45.0f, 0.0f } );

	cameraEntity->addComponent( cameraComponent );

	WorldSector* sector = WV_NEW( WorldSector );

	// Create meshes
	for ( size_t i = 0; i < 10; i++ )
	{
		MeshComponent* meshComponent = WV_NEW( MeshComponent );
		meshComponent->setRenderMesh( mesh );

		Entity* entity = WV_NEW( Entity );
		entity->getTransform().setPosition( { (float)i * 2.1f, std::sinf( i ), 0.0f });
		entity->addComponent( meshComponent );
		sector->addEntity( entity );
	}
	
	sector->addEntity( cameraEntity );
	m_world->addSector( sector );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::shutdown()
{
	m_world->shutdown();
	WV_FREE( m_world );

	m_renderer.destroyMaterial( m_material );

	m_renderer.shutdown();
	m_displayDriver->shutdown();

	WV_FREE( m_inputSystem );
	WV_FREE( m_eventManager );

	ReflectionRegistry::destroySingleton();
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Application::tick()
{
	if ( !m_alive )
		return false;

	m_displayDriver->swapBuffers();
	
	m_inputSystem->processInputEvents( m_eventManager );
	m_eventManager->processEvents();

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
	wv::Vector2i windowSize = m_displayDriver->getWindowSize(); // should this be an event?
	m_world->getViewport()->setSize( windowSize.x, windowSize.y );

	m_world->updateLoading();
	m_world->updateSectors( m_deltatime );
	m_world->updateWorldSystems( m_deltatime );

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
