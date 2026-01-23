#include "application.h"


#include <wv/camera/components/orbit_camera_component.h>
#include <wv/camera/systems/camera_manager_system.h>
#include <wv/display_driver.h>
#include <wv/entity/entity.h>
#include <wv/entity/world.h>
#include <wv/entity/world_sector.h>
#include <wv/event/event_manager.h>
#include <wv/filesystem/file_system.h>
#include <wv/rendering/renderer.h>
#include <wv/rendering/material.h>
#include <wv/rendering/systems/render_world_system.h>
#include <wv/rendering/components/mesh_component.h>
#include <wv/rendering/viewport.h>
#include <wv/reflection/reflection.h>
#include <wv/input/input_system.h>
#include <wv/input/components/player_input_component.h>
#include <wv/input/systems/player_input_system.h>
#include <wv/math/math.h>
#include <wv/memory/memory.h>
#include <wv/platform/platform.h>

#include <wv/gametest/player_component.h>
#include <wv/gametest/player_controller.h>

// TODO: MOVE TO WINDOWS DRIVER PLACE SOMEWHERE
#include <windows/xinput_controller_driver.h>
#include <windows/windows_keyboard_driver.h>
#include <windows/windows_mouse_driver.h>

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
	m_graphicsDriverName = "vulkan"; // TODO
	m_world = _world;

	// IEngineSystem ?
	// might help with cleanup and such
	// systemManager->createSystem<DisplayDriver>( Platform::createDisplayDriver() );
	// systemManager->createSystem<Renderer>( Platform::createRenderer() );
	// systemManager->createSystem<FileSystem>( Platform::createFileSystem( "data" ) );
	// systemManager->createSystem<InputSystem>();
	// systemManager->initialize();

	m_eventManager = WV_NEW( EventManager );
	m_inputSystem  = WV_NEW( InputSystem );
	m_filesystem   = Platform::createFileSystem( "data" );

	m_inputSystem->createInputDriver<XInputControllerDriver>();
	m_inputSystem->createInputDriver<WindowsKeyboardDriver>();
	m_inputSystem->createInputDriver<WindowsMouseDriver>();

	m_inputSystem->initialize();

	m_displayDriver = Platform::createDisplayDriver();

	if ( !m_displayDriver->initializeDisplay( _windowWidth, _windowHeight ) )
	{
		WV_LOG_ERROR( "Failed to initalize display\n" );
		WV_FREE( m_displayDriver );
		return false;
	}

	m_renderer = WV_NEW( Renderer );

	if ( !m_renderer->initialize() )
	{
		WV_LOG_ERROR( "Failed to initalize renderer\n" );
		shutdown();
		return false;
	}

	ActionGroup* playerActionGroup = m_inputSystem->createActionGroup( "Player" );

	playerActionGroup->bindTriggerAction( "Jump", "Controller", CONTROLLER_BUTTON_A );
	playerActionGroup->bindTriggerAction( "Jump", "Keyboard", SCANCODE_SPACE );
	
	playerActionGroup->bindAxisAction( "Move", "Controller", AXIS_DIRECTION_ALL, CONTROLLER_JOYSTICK_LEFT );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", AXIS_DIRECTION_NORTH, SCANCODE_W );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", AXIS_DIRECTION_SOUTH, SCANCODE_S );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", AXIS_DIRECTION_EAST, SCANCODE_D );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", AXIS_DIRECTION_WEST, SCANCODE_A );
	
	playerActionGroup->enable();

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
	// Set up world

	m_world->createWorldSystem<RenderWorldSystem>();
	m_world->createWorldSystem<CameraManagerSystem>();
	PlayerInputSystem* playerInputSystem = m_world->createWorldSystem<PlayerInputSystem>();
	playerInputSystem->setSelectionMode( PlayerInputSystem::SelectionMode::ANY_TRIGGER_ACTION );

	Entity* cameraEntity = WV_NEW( Entity );
	cameraEntity->createComponent<PlayerInputComponent>()->setPlayerIndex( 0 );
	cameraEntity->createComponent<OrbitCameraComponent>();
	
	cameraEntity->getTransform().setPosition( { 0, 10.0f, 10.0f } );
	cameraEntity->getTransform().setRotation( { -30.0f, 0.0f, 0.0f } );

	//m_triangleMaterialType.addSpan( "worldMatrix", UNIFORM_TYPE_MATRIX );
	//m_triangleMaterialType.addSpan( "positionBuffer", UNIFORM_TYPE_BUFFER_ADDRESS );
	//m_triangleMaterialType.addSpan( "vertexBuffer", UNIFORM_TYPE_BUFFER_ADDRESS );
	
	std::vector<uint8_t> vertShaderData = m_filesystem->loadEntireFile( "shaders/coloured_triangle.vert.spv" );
	std::vector<uint8_t> fragShaderData = m_filesystem->loadEntireFile( "shaders/coloured_triangle.frag.spv" );
	ResourceID pipeline = m_renderer->createPipeline( (uint32_t*)vertShaderData.data(), vertShaderData.size(), (uint32_t*)fragShaderData.data(), fragShaderData.size() );

	m_testMaterial = WV_NEW( MaterialType, pipeline );
	m_testMaterial->addSpan( "albedoIndex", UNIFORM_TYPE_TEXTURE );

	Entity* playerEntity1 = WV_NEW( Entity );
	{
		MeshComponent* meshComponent = playerEntity1->createComponent<MeshComponent>();
		meshComponent->setFilePath( "monkey.gltf" );
		meshComponent->setMaterial( m_testMaterial );
		meshComponent->setMaterialValue( 0, "albedoIndex", 0 );

		playerEntity1->createComponent<PlayerInputComponent>()->setPlayerIndex( 0 );
		playerEntity1->createComponent<PlayerControllerComponent>();
		playerEntity1->createSystem<PlayerControllerSystem>();

		playerEntity1->getTransform().setPosition( { -3.0f, 0.0f, 0.0 } );
	}
	
	Entity* playerEntity2 = WV_NEW( Entity );
	{
		MeshComponent* meshComponent = playerEntity2->createComponent<MeshComponent>();
		meshComponent->setFilePath( "meshes/SM_Suzanne.gltf" );
		meshComponent->setMaterial( m_testMaterial );
		meshComponent->setMaterialValue( 0, "albedoIndex", 1 );

		playerEntity2->createComponent<PlayerInputComponent>()->setPlayerIndex( 1 );
		playerEntity2->createComponent<PlayerControllerComponent>();
		playerEntity2->createSystem<PlayerControllerSystem>();

		playerEntity2->getTransform().setPosition( { 3.0f, 0.0f, 0.0 } );
	}
	
	Entity* materialEntity = WV_NEW( Entity );
	{
		MeshComponent* meshComponent = materialEntity->createComponent<MeshComponent>();
		meshComponent->setFilePath( "meshes/SM_MaterialSphere.glb" );
		meshComponent->setMaterial( m_testMaterial );
		meshComponent->setMaterialValue( 0, "albedoIndex", 2 );

		materialEntity->getTransform().setPosition( { 0.0f, 0.0f, 0.0f } );
	}

	WorldSector* sector = WV_NEW( WorldSector );

	sector->addEntity( cameraEntity );
	sector->addEntity( playerEntity1 );
	sector->addEntity( playerEntity2 );
	sector->addEntity( materialEntity );
	m_world->addSector( sector );

	m_lastTicks = m_displayDriver->getHighResolutionCounter();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::shutdown()
{
	m_renderer->waitForRenderer();

	if ( m_world )
	{
		m_world->shutdown();
		WV_FREE( m_world );
	}
	
	m_renderer->destroyPipeline( m_testMaterial->getPipeline() );
	WV_FREE( m_testMaterial );

	if ( m_displayDriver )
	{
		m_displayDriver->shutdown();
		WV_FREE( m_displayDriver );
	}

	if ( m_inputSystem )
	{
		m_inputSystem->shutdown();
		WV_FREE( m_inputSystem );
	}
	
	if ( m_eventManager )
	{
		WV_FREE( m_eventManager );
	}
	
	if ( m_renderer )
	{
		m_renderer->shutdown();
		WV_FREE( m_renderer );
	}
	
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
	m_deltatime = wv::Math::min( m_deltatime, 1.0 ); // hard cap just in case

	update();
	render();

	m_lastTicks = ticks;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::update()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();
	if ( windowSize.x == 0 ) windowSize.x = 1;
	if ( windowSize.y == 0 ) windowSize.y = 1;

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
	bool shouldRender = true;
	bool isMinimized = m_displayDriver->isMinimized();
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	// don't render if the window is minimized
	if ( isMinimized || windowSize.x == 0 || windowSize.y == 0 )
		shouldRender = false;

	if ( m_renderer->isSwapchainOutOfDate() )
	{
		if ( !isMinimized )
			m_renderer->resizeSwapchain( windowSize.x, windowSize.y );
	}

	if ( shouldRender )
	{
		m_renderer->prepare( windowSize.x, windowSize.y );
		m_renderer->render( m_world );
		m_renderer->finalize();
	}
}
