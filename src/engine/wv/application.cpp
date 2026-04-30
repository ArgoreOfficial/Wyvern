#include "application.h"

#include <wv/audio/audio_system.h>

#include <wv/debug/timer.h>

#include <wv/display_driver.h>

#include <wv/editor/editor_interface_system.h>
#include <wv/editor/editor_camera_system.h>
#include <wv/editor/editor_transform_system.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/event/event_manager.h>

#include <wv/filesystem/file_system.h>

#include <wv/input/input_system.h>

#include <wv/rendering/renderer.h>
#include <wv/rendering/material.h>
#include <wv/rendering/viewport.h>

#include <wv/reflection/reflection.h>

#include <wv/shaders/lit_shader.h>
#include <wv/shaders/unlit_shader.h>
#include <wv/shaders/debug_shader.h>

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

#include <nlohmann/json.hpp>

#include <fstream>
#include <set>

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
	m_activeWorld = _world;

	m_meshManager = WV_NEW( MeshManager );
	m_materialManager = WV_NEW( MaterialManager );
	m_textureManager = WV_NEW( TextureManager );
	m_shaderManager = WV_NEW( ShaderManager );

	m_eventManager = WV_NEW( EventManager );
	m_inputSystem  = WV_NEW( InputSystem );
	m_filesystem   = Platform::createFileSystem( "data" );
	
	m_taskSystem = WV_NEW( TaskSystem );
	m_taskSystem->createThreads( 20 );

	m_inputSystem->createInputDriver<XInputControllerDriver>();
	m_inputSystem->createInputDriver<WindowsKeyboardDriver>();
	IMouseDriver* mouseDriver = m_inputSystem->createInputDriver<WindowsMouseDriver>();

	m_inputSystem->initialize( mouseDriver );

	m_displayDriver = Platform::createDisplayDriver();

	if ( !m_displayDriver->initializeDisplay( _windowWidth, _windowHeight ) )
	{
		WV_LOG_ERROR( "Failed to initialize display\n" );
		WV_FREE( m_displayDriver );
		return false;
	}
	
	m_audioSystem = Platform::createAudioSystem();
	m_audioSystem->initialize();

	m_renderer = WV_NEW( Renderer );

	if ( !m_renderer->initialize() )
	{
		WV_LOG_ERROR( "Failed to initialize renderer\n" );
		shutdown();
		return false;
	}

	///////////////////////////////////////////////////////////////////////////
	// Set up camera

	wv::Vector2i windowSize = m_displayDriver->getWindowSize();

	Viewport* viewport = m_activeWorld->getViewport();
	if ( !viewport )
	{
		viewport = WV_NEW( Viewport );
		m_activeWorld->setViewport( viewport );
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Set up shaders
	
	{
		wv::Ref<IShader> litShader = std::make_shared<LitShader>();
		litShader->initialize();
		m_shaderManager->add( "lit", litShader );
	}
	
	{
		wv::Ref<IShader> litShader = std::make_shared<UnlitShader>();
		litShader->initialize();
		m_shaderManager->add( "unlit", litShader );
	}
	
	{
		wv::Ref<IShader> litShader = std::make_shared<DebugShader>();
		litShader->initialize();
		m_shaderManager->add( "debug", litShader );
	}

	///////////////////////////////////////////////////////////////////////////
	// Set up default assets

	// Default lit material
	{
		wv::Ref<MaterialAsset> material = m_materialManager->get( "materials/default_lit.wvmt" );
		m_materialManager->makePersistent( material );
	}

	// Default unlit material
	{
		wv::Ref<MaterialAsset> material = m_materialManager->get( "materials/default_unlit.wvmt" );
		m_materialManager->makePersistent( material );
	}

	// Default debug material
	{
		wv::Ref<MaterialAsset> material = m_materialManager->get( "materials/default_debug.wvmt" );
		m_materialManager->makePersistent( material );
	}

	///////////////////////////////////////////////////////////////////////////
	// Editor features

#ifndef WV_PACKAGE
	wv::ActionGroup* editorActionGroup = m_inputSystem->createActionGroup( "Editor" );
	editorActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_NORTH, wv::SCANCODE_W );
	editorActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_SOUTH, wv::SCANCODE_S );
	editorActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_EAST, wv::SCANCODE_D );
	editorActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_WEST, wv::SCANCODE_A );
	
	editorActionGroup->bindTriggerAction( "MouseLeft", "Mouse", wv::MOUSE_BUTTON_LEFT );
	editorActionGroup->bindTriggerAction( "MouseRight", "Mouse", wv::MOUSE_BUTTON_RIGHT );

	editorActionGroup->bindTriggerAction( "Translate", "Keyboard", wv::SCANCODE_G );
	editorActionGroup->bindTriggerAction( "Rotate",    "Keyboard", wv::SCANCODE_R );
	editorActionGroup->bindTriggerAction( "Scale",     "Keyboard", wv::SCANCODE_S );

	editorActionGroup->bindTriggerAction( "LockAxisX", "Keyboard", wv::SCANCODE_X );
	editorActionGroup->bindTriggerAction( "LockAxisY", "Keyboard", wv::SCANCODE_Y );
	editorActionGroup->bindTriggerAction( "LockAxisZ", "Keyboard", wv::SCANCODE_Z );
	
	editorActionGroup->bindValueAction( "ShiftLeft", "Keyboard", wv::SCANCODE_LEFT_SHIFT );
	
	editorActionGroup->disable(); // default off

	m_activeWorld->registerComponentType<EditorObjectComponent>( "EditorObjectComponent" );
	m_activeWorld->registerComponentType<EditorCameraComponent>( "EditorCameraComponent" );

	m_activeWorld->addSystem<EditorInterfaceSystem>();
	m_activeWorld->addSystem<EditorCameraSystem>();
	m_activeWorld->addSystem<EditorTransformSystem>();
#endif

	///////////////////////////////////////////////////////////////////////////
	// Set up world

	m_activeWorld->onSetupInput( m_inputSystem );

	// systems must be set up first

	m_activeWorld->addSystem<MeshRenderSystem>();
	m_activeWorld->addSystem<PhysicsSystem>();
	m_activeWorld->addSystem<CameraManagerSystem>();
	m_activeWorld->addSystem<OrbitControllerSystem>();

	m_activeWorld->onSceneCreate();

	m_activeWorld->updateFrameData( 0.0f, 0.0f );
	m_activeWorld->dispatchUpdateMessage( UpdateEvent_initialize );
	m_activeWorld->updateComponentChanges();
	
	// Update material buffers, as new materials have likely been loaded during world creation

	m_shaderManager->updateBuffers();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Application::shutdown()
{
	m_activeWorld->unload( true );
	m_activeWorld->dispatchUpdateMessage( UpdateEvent_shutdown );

	m_renderer->waitForRenderer();

	if ( m_activeWorld )
	{
		WV_FREE( m_activeWorld );
	}
	
	if ( m_meshManager )
	{
		m_meshManager->clearPersistent();
		WV_FREE( m_meshManager );
	}
	
	if ( m_materialManager )
	{
		m_materialManager->clearPersistent();
		WV_FREE( m_materialManager );
	}

	if ( m_textureManager )
	{
		m_textureManager->clearPersistent();
		WV_FREE( m_textureManager );
	}

	if ( m_shaderManager )
		WV_FREE( m_shaderManager );
	
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
	
		if ( isActionsEnabled )
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

void wv::Application::setCursorLock( bool _lock )
{
	m_displayDriver->setCursorLock( _lock );
}

void wv::Application::setCursorVisible( bool _visible )
{
	m_displayDriver->setCursorVisible( _visible );
}

void wv::Application::update()
{
	wv::Vector2i windowSize = m_displayDriver->getWindowSize();
	if ( windowSize.x == 0 ) windowSize.x = 1;
	if ( windowSize.y == 0 ) windowSize.y = 1;
	m_activeWorld->getViewport()->size = { windowSize.x, windowSize.y };

	m_activeWorld->updateFrameData( m_deltatime, m_fixedDeltaTime );
	m_activeWorld->updateComponentChanges();
	
	// physics update

	if ( m_activeWorld->isRuntimeState() )
	{
		PhysicsSystem* physicsSystem = m_activeWorld->m_ecsEngine->getSystem<PhysicsSystem>();
		
		physicsSystem->onInternalPrePhysicsUpdate();

		m_accumulator += m_deltatime;
		while ( m_accumulator > m_fixedDeltaTime )
		{
			m_activeWorld->dispatchUpdateMessage( UpdateEvent_physicsUpdate );


			physicsSystem->onInternalPhysicsUpdate( m_fixedDeltaTime );

			m_fixedRuntime += m_fixedDeltaTime;
			m_accumulator -= m_fixedDeltaTime;
		}

		physicsSystem->onInternalPostPhysicsUpdate( m_accumulator / m_fixedDeltaTime );
	}

	// normal update

	m_activeWorld->dispatchUpdateMessage( UpdateEvent_preUpdate );
	m_activeWorld->dispatchUpdateMessage( UpdateEvent_update );
	m_activeWorld->dispatchUpdateMessage( UpdateEvent_postUpdate );

	for ( Entity* entity : m_activeWorld->m_entities )
	{
		// if an entity has a parent, it will be updated through the parents update() function
		if ( entity->getParent() )
			continue;

		entity->update( nullptr, true );
	}

	CameraManagerSystem* cameraSystem = m_activeWorld->m_ecsEngine->getSystem<CameraManagerSystem>();
	cameraSystem->onInternalCameraUpdate();

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
		m_activeWorld->dispatchUpdateMessage( UpdateEvent_debugRender );
		
		m_renderer->beginDebugRender();
		m_activeWorld->dispatchUpdateMessage( UpdateEvent_editorRender );
		m_renderer->endDebugRender();
	}
#endif

	if ( shouldRender )
	{
		m_activeWorld->dispatchUpdateMessage( UpdateEvent_render );
		m_shaderManager->updateBuffers();
		m_renderer->render( m_activeWorld );
	}
	
}
