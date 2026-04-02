#include "application.h"

#include <wv/audio/audio_system.h>

#include <wv/debug/timer.h>

#include <wv/display_driver.h>

#include <wv/editor/editor_interface_system.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/event/event_manager.h>

#include <wv/input/input_system.h>

#include <wv/rendering/renderer.h>
#include <wv/rendering/material.h>
#include <wv/rendering/viewport.h>

#include <wv/reflection/reflection.h>

#include <wv/systems/mesh_render_system.h>
#include <wv/systems/camera_manager_system.h>
#include <wv/systems/orbit_controller_system.h>
#include <wv/systems/physics_system.h>

#include <wv/math/math.h>
#include <wv/memory/memory.h>
#include <wv/platform/platform.h>
#include <wv/thread/thread.h>
#include <wv/thread/task_system.h>

// TODO: MOVE TO WINDOWS DRIVER PLACE SOMEWHERE
#include <windows/xinput_controller_driver.h>
#include <windows/windows_keyboard_driver.h>
#include <windows/windows_mouse_driver.h>

#include <tracy/Tracy.hpp>

#ifdef WV_SUPPORT_IMGUI
#include <imgui/imgui.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

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

	m_eventManager = WV_NEW( EventManager );
	m_inputSystem  = WV_NEW( InputSystem );
	m_filesystem   = Platform::createFileSystem( "data" );
	
	m_taskSystem = WV_NEW( TaskSystem );
	m_taskSystem->createThreads( 20 );

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
	
	m_audioSystem = Platform::createAudioSystem();
	m_audioSystem->initialize();

	m_renderer = WV_NEW( Renderer );

	if ( !m_renderer->initialize() )
	{
		WV_LOG_ERROR( "Failed to initalize renderer\n" );
		shutdown();
		return false;
	}

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
	// Set up default assets

	MaterialManager* materialManager = m_world->getMaterialManager();

	// Default lit material
	{
		wv::Ref<MaterialAsset> material = std::make_shared<MaterialAsset>();
		auto def = material->deserialize( "shaders/default_lit.vert.spv", "shaders/default_lit.frag.spv" );

		def.uniforms = {
			{ "albedoIndex", sizeof( uint32_t ) },
			{ "albedoColor", sizeof( Vector4f ) }
		};
		
		material->initialize( def );

		materialManager->add( "Default Lit", material );
		materialManager->makePersistent( material );
	}

	// Default unlit material
	{
		wv::Ref<MaterialAsset> material = std::make_shared<MaterialAsset>();
		auto def = material->deserialize( "shaders/default_unlit.vert.spv", "shaders/default_unlit.frag.spv" );

		def.uniforms = {
			{ "albedoIndex", sizeof( uint32_t ) },
			{ "albedoColor", sizeof( Vector4f ) }
		};
		
		material->initialize( def );

		materialManager->add( "Default Unlit", material );
		materialManager->makePersistent( material );
	}

	// Default debug material
	{
		wv::Ref<MaterialAsset> material = std::make_shared<MaterialAsset>();
		auto def = material->deserialize( "shaders/default_debug.vert.spv", "shaders/default_debug.frag.spv" );
		
		def.uniforms = { 
			{ "color", sizeof( wv::Vector4f ) } 
		};
		
		def.topology = TopologyClass::WV_LINE;

		material->initialize( def );

		materialManager->add( "Debug", material );
		materialManager->makePersistent( material );
	}

	///////////////////////////////////////////////////////////////////////////
	// Set up world

	m_world->onSetupInput( m_inputSystem );

	// systems must be set up first
	m_world->addSystem<MeshRenderSystem>();
	m_world->addSystem<PhysicsSystem>();
	m_world->addSystem<CameraManagerSystem>();
	m_world->addSystem<OrbitControllerSystem>();

	m_world->onSceneCreate();
	m_world->updateComponentChanges();

	m_world->dispatchUpdateMessage( UpdateMessageType_initialize );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::shutdown()
{
	m_world->dispatchUpdateMessage( UpdateMessageType_shutdown );

	m_renderer->waitForRenderer();

	if ( m_world )
	{
		WV_FREE( m_world );
	}
	
	if ( m_taskSystem )
	{
		Debug::Print( Debug::WV_PRINT_DEBUG, "Waiting for threads\n" );
		m_taskSystem->shutdownThreads();
		WV_FREE( m_taskSystem );
	}

	if ( m_audioSystem )
	{
		m_audioSystem->shutdown();
		WV_FREE( m_audioSystem );
	}

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

	wv::Timer timer{};
	
	// Input & Actions
	{
		const bool isActionsEnabled = m_inputSystem->getActionsEnabled();
		const bool isFocused = m_displayDriver->isFocused();

		if ( isActionsEnabled )
			m_inputSystem->setActionsEnabled( isFocused );
	
		m_inputSystem->processInputEvents( m_eventManager );
	
		if( isActionsEnabled )
			m_inputSystem->setActionsEnabled( true );
	}

	m_eventManager->processEvents();

	m_audioSystem->updateRecordingDevices();
	
	update();
	render();
	
	// Update runtime and deltatime
	m_deltatime = timer.elapsed();
	m_runtime += m_deltatime;
	
	m_deltatime = wv::Math::clamp( m_deltatime, 0.00001, 1.0 ); // hard cap just in case

	FrameMark;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::update()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();
	if ( windowSize.x == 0 ) windowSize.x = 1;
	if ( windowSize.y == 0 ) windowSize.y = 1;
	m_world->getViewport()->size = { windowSize.x, windowSize.y };

	// adds and removes components
	m_world->updateComponentChanges();
	// updates archetype lists. Move to only update when modified?
	m_world->m_ecsEngine->updateSystemsArchetypes();

	// physics update

	wv::PhysicsSystem* physicsSystem = m_world->m_ecsEngine->getSystem<PhysicsSystem>();
	physicsSystem->onInternalPrePhysicsUpdate();

	m_accumulator += m_deltatime;
	while ( m_accumulator > m_fixedDeltaTime )
	{
		physicsSystem->onInternalPhysicsUpdate( m_fixedDeltaTime );

		m_fixedRuntime += m_fixedDeltaTime;
		m_accumulator -= m_fixedDeltaTime;
	}

	// normal update

	m_world->dispatchUpdateMessage( UpdateMessageType_preUpdate );

	m_world->dispatchUpdateMessage( UpdateMessageType_update );
	for ( Entity* entity : m_world->m_entities )
		entity->getTransform().update( nullptr );

	m_world->dispatchUpdateMessage( UpdateMessageType_postUpdate );
	
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

#ifdef WV_DEBUG
	if ( shouldRender )
	{
		m_renderer->beginDebugRender();
		
		m_world->dispatchUpdateMessage( UpdateMessageType_debugRender );

		m_renderer->endDebugRender();
	}
#endif

	if ( shouldRender )
	{
		m_world->dispatchUpdateMessage( UpdateMessageType_render );
		m_renderer->render( m_world );
	}
	
}
