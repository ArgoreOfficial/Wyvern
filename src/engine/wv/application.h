#pragma once

#include <wv/debug/error.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>

#include <wv/resource_id.h>
#include <wv/types.h>

#include <chrono>

namespace wv {

class IFileSystem;
class DisplayDriver;
class World;
class InputSystem;
class EventManager;
class Renderer;
class TaskSystem;
class AudioSystem;

class MaterialAsset;
class MaterialManager;
class ShaderManager;

class MeshManager;

struct VertexData
{
	wv::Vector3f normal;
	wv::Vector3f color;
	wv::Vector2f texCoord;
};

class Application 
{
public:
	Application();
	
	static Application* getSingleton() { return Application::singleton; }

	bool initialize( World* _world, int _windowWidth, int _windowHeight );
	void shutdown();

	bool tick();

	inline double getApplicationTime ( void ) const { return m_runtime; }
	inline double getDeltaTime       ( void ) const { return m_deltatime; }
	inline double getPhysicsDeltaTime( void ) const { return m_fixedDeltaTime; }
	inline double getPhysicsFraction ( void ) const { return m_accumulator / m_fixedDeltaTime; }

	inline std::string    getGraphicsDriverName() const { return m_graphicsDriverName; }
	inline DisplayDriver* getDisplayDriver()      const { return m_displayDriver; }
	inline IFileSystem*   getFileSystem()         const { return m_filesystem; }
	inline InputSystem*   getInputSystem()        const { return m_inputSystem; }
	inline EventManager*  getEventManager()       const { return m_eventManager; }
	inline Renderer*      getRenderer()           const { return m_renderer; }
	inline TaskSystem*    getTaskSystem()         const { return m_taskSystem; }
	inline AudioSystem*   getAudioSystem()        const { return m_audioSystem; }
	inline World*         getWorld()              const { return m_world; }

	MeshManager* getMeshManager() const { return m_meshManager; }
	ShaderManager* getShaderManager() const { return m_shaderManager; }
	MaterialManager* getMaterialManager() const { return m_materialManager; }

	void quit() { m_alive = false; }

	// helpers

	void setCursorLock( bool _lock );
	void setCursorVisible( bool _visible );

private:
	
	static Application* singleton;

	void update();
	void render();

	std::string m_graphicsDriverName = "opengl";

	bool m_alive = true;

	double m_runtime = 0.0;
	double m_deltatime = 1.0 / 60.0;

	const double m_fixedDeltaTime = 0.02;
	double m_fixedRuntime = 0.0;
	double m_accumulator = 0.0;

	// Subsystems

	wv::DisplayDriver*  m_displayDriver = nullptr;
	wv::IFileSystem*    m_filesystem    = nullptr;
	wv::InputSystem*    m_inputSystem   = nullptr;
	wv::EventManager*   m_eventManager  = nullptr;
	wv::Renderer*       m_renderer      = nullptr;
	wv::TaskSystem*     m_taskSystem    = nullptr;
	wv::AudioSystem*    m_audioSystem   = nullptr;

	// Managers
	
	wv::MeshManager* m_meshManager = nullptr;
	wv::MaterialManager* m_materialManager = nullptr;
	wv::ShaderManager* m_shaderManager = nullptr;

	wv::World* m_world = nullptr;
};

static Application* getApp() {
	return wv::Application::getSingleton();
}

}
