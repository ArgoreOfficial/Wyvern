#include "application.h"

#include <wv/audio/audio_system.h>

#include <wv/debug/timer.h>

#include <wv/display_driver.h>

#include <wv/editor/editor_interface_system.h>

#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/event/event_manager.h>

#include <wv/rendering/renderer.h>
#include <wv/rendering/material.h>

#include <wv/rendering/viewport.h>
#include <wv/reflection/reflection.h>
#include <wv/input/input_system.h>

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

#include <bitset>

#ifdef WV_SUPPORT_IMGUI
#include <imgui/imgui.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

class Archetype;
class ECSEngine;
class ISystem;
struct IComponentVector;

struct ArchetypeConfig
{
	ECSEngine* engine;
	std::vector<int> componentTypeIndices{};
	std::vector<IComponentVector*> componentContainers{};

	template<typename Ty>
	void addComponentType();

	std::bitset<256> getBitmask() {
		std::bitset<256> bitmask{};
		for ( int i : componentTypeIndices )
		{
			if ( i < 0 )
				continue;
			bitmask[ i ] = true;
		}
		return bitmask;
	}

private:
	friend class ECSEngine;

	void freeContainers() {
		for ( size_t i = 0; i < componentContainers.size(); i++ )
			WV_FREE( componentContainers[ i ] );
	}

};

class ECSEngine
{
public:
	template<typename Ty>
	struct ComponentTypeDef { static inline int index = -1; };

	struct ArchetypeTypeDef
	{
		std::bitset<256> bitmask;
		Archetype* archetype;
	};

	int numComponentTypes = 0;

	template<typename Ty>
	int addComponentType()
	{
		if ( ComponentTypeDef<Ty>::index == -1 )
			ComponentTypeDef<Ty>::index = numComponentTypes++;

		return ComponentTypeDef<Ty>::index;
	}

	template<typename Ty>
	Ty* addSystem();

	void registerArchetype( ArchetypeConfig& _config );

	void updateSystems();

	std::vector<ArchetypeTypeDef> m_archetypes;
	std::vector<ISystem*> m_systems;
};

struct IComponentVector { }; // helper
template<typename Ty>
struct ComponentVector
{
	std::vector<Ty> components;
	std::vector<Entity*> entities;
};

template<typename Ty>
void ArchetypeConfig::addComponentType()
{
	int componentTypeIndex = engine->addComponentType<Ty>();

	for ( size_t i = 0; i < componentTypeIndices.size(); i++ )
		if ( componentTypeIndices[ i ] == componentTypeIndex )
			return;
	
	componentTypeIndices.push_back( componentTypeIndex );
	componentContainers.push_back( (IComponentVector*)WV_NEW( ComponentVector<Ty> ) );
}

struct Archetype
{
	std::unordered_map<int, IComponentVector*> m_vectors;

	template<typename Ty>
	std::vector<Ty>& getComponents() {
		int componentTypeIndex = ECSEngine::ComponentTypeDef<Ty>::index;
		WV_ASSERT_MSG( componentTypeIndex >= 0, "Component type not registered" );
		
		ComponentVector<Ty>* vec = reinterpret_cast<ComponentVector<Ty>*>( m_vectors.at( componentTypeIndex ) );
		return vec->components;
	}
};

class ISystem
{
public:
	std::vector<Archetype*>& getArchetypes() {
		return m_archetypes;
	}

	std::bitset<256> getArchetypeBitmask() const { return m_archetypeBitmask; }

	virtual void configure( ArchetypeConfig& _config ) = 0;
	virtual void update() = 0;

private:
	friend class ECSEngine;

	std::bitset<256> m_archetypeBitmask{};
	std::vector<Archetype*> m_archetypes;
};

template<typename Ty>
Ty* ECSEngine::addSystem() {
	static_assert( std::is_base_of<ISystem, Ty>(), "Type must derive from wv::ISystem" );

	ISystem* s = WV_NEW( Ty );
	ArchetypeConfig config{};
	config.engine = this;
	s->configure( config );

	s->m_archetypeBitmask = config.getBitmask();
	registerArchetype( config );

	m_systems.push_back( s );

	return (Ty*)s;
}

void ECSEngine::registerArchetype( ArchetypeConfig& _config )
{
	auto bitmask = _config.getBitmask();
	if ( bitmask.none() )
		return; // empty config

	for ( size_t i = 0; i < m_archetypes.size(); i++ )
	{
		if ( m_archetypes[ i ].bitmask != bitmask )
			continue;

		_config.freeContainers();
		return; // exact match found, archetype already exists
	}


	// create new archetype

	Archetype* archetype = WV_NEW( Archetype );

	for ( size_t i = 0; i < _config.componentTypeIndices.size(); i++ )
	{
		// give ownership of the container 
		archetype->m_vectors.emplace(
			_config.componentTypeIndices[ i ],
			_config.componentContainers[ i ]
		);
	}
	
	m_archetypes.emplace_back( bitmask, archetype );
}

void ECSEngine::updateSystems()
{
	for ( ISystem* s : m_systems )
	{
		s->m_archetypes.clear();

		std::bitset<256> bitmask = s->getArchetypeBitmask();
		if ( bitmask.any() )
		{
			for ( auto& archetype : m_archetypes )
			{
				if ( ( archetype.bitmask & bitmask ).any() )
					s->m_archetypes.push_back( archetype.archetype );
			}
		}

		s->update();
	}
}

struct TestComponent
{
	int value;
};

struct TestComponentDifferent
{
	float foo;
	bool bar;
};

class TestSystem : public wv::ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override
	{
		_config.addComponentType<TestComponent>();
		_config.addComponentType<TestComponentDifferent>();
	}

	virtual void update() override
	{
		if ( ImGui::Begin( "TestSystem" ) )
		{
			for ( auto archetype : getArchetypes() )
			{
				ImGui::Text( "Archetype" );
				
				std::vector<TestComponent>&          testComps  = archetype->getComponents<TestComponent>();
				std::vector<TestComponentDifferent>& testCompsD = archetype->getComponents<TestComponentDifferent>();

				for ( size_t i = 0; i < testComps.size(); i++ )
				{
					ImGui::Text( "%i . %f, %s", testComps[ i ].value, testCompsD[ i ].foo, testCompsD[ i ].bar ? "true" : "false" );
				}
			}
		}
		ImGui::End();
	}
};

}

///////////////////////////////////////////////////////////////////////////////////////

wv::Application* wv::Application::singleton = nullptr;

static wv::ECSEngine ecsEngine;
static wv::TestSystem* testSystem;

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
	
	m_taskSystem = WV_NEW( TaskSystem );
	m_taskSystem->createThreads( 20 );

	Debug::Print( "Wait complete\n" );

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

	m_world->createWorldSystem<EditorInterfaceSystem>();

	m_world->onSceneCreate();

	testSystem = ecsEngine.addSystem<TestSystem>();



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

#ifdef WV_DEBUG
	if ( shouldRender )
	{
		
		m_renderer->beginDebugRender();

		// Don't render debug if the window isn't focused
		// This should be togglable
		if( m_displayDriver->isFocused() )
			m_world->onDebugRender();

		ecsEngine.updateSystems();

		m_renderer->endDebugRender();
	}
#endif

	if ( shouldRender )
		m_renderer->render( m_world );
	
}
