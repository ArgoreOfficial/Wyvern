#pragma once

#include <wv/math/vector3.h>
#include <wv/camera/view_volume.h>
#include <wv/debug/error.h>

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

class MaterialAsset;

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

	inline double getApplicationTime( void ) const { return m_runtime; }
	inline double getDeltaTime      ( void ) const { return m_deltatime; }

	inline std::string    getGraphicsDriverName() const { return m_graphicsDriverName; }
	inline DisplayDriver* getDisplayDriver()      const { return m_displayDriver; }
	inline IFileSystem*   getFileSystem()         const { return m_filesystem; }
	inline InputSystem*   getInputSystem()        const { return m_inputSystem; }
	inline EventManager*  getEventManager()       const { return m_eventManager; }
	inline Renderer*      getRenderer()           const { return m_renderer; }
	inline TaskSystem*    getTaskSystem()         const { return m_taskSystem; }
	inline World*         getWorld()              const { return m_world; }

	void quit() { m_alive = false; }
private:
	
	static Application* singleton;

	void update();
	void render();

	std::string m_graphicsDriverName = "opengl";

	bool m_alive = true;

	double m_runtime = 0.0;
	double m_deltatime = 1.0 / 60.0;

	const double m_fixed_delta_time = 0.01;
	double m_fixed_runtime = 0.0;
	double m_accumulator = 0.0;

	// Subsystems

	wv::DisplayDriver*  m_displayDriver = nullptr;
	wv::IFileSystem*    m_filesystem    = nullptr;
	wv::InputSystem*    m_inputSystem   = nullptr;
	wv::EventManager*   m_eventManager  = nullptr;
	wv::Renderer*       m_renderer      = nullptr;
	wv::TaskSystem*     m_taskSystem    = nullptr;

	wv::World* m_world = nullptr;
};

}
