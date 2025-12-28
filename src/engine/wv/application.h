#pragma once

#include <wv/math/vector3.h>
#include <wv/graphics/renderer.h>
#include <wv/camera/view_volume.h>
#include <wv/debug/error.h>

namespace wv {

class IFileSystem;
class DisplayDriver;
class World;

struct VertexData
{
	wv::Vector3f normal;
	wv::Vector3f color;
	wv::Vector2f texCoord;
};

class InputSystem
{
public:
	InputSystem() = default;

	void updateInputDrivers();
	void processInputEvents();

#ifndef WV_PACKAGE
	// DEBUG ONLY 
	inline wv::Vector2f debugGetMouseMotion() const { return m_debugMouseMotion; }

	// DEBUG ONLY 
	inline wv::Vector2f debugGetMousePosition() const { return m_debugMousePosition; }

	// DEBUG ONLY 
	inline bool debugIsMouseDown( int _index ) {
		WV_ASSERT( _index < 0 );
		WV_ASSERT( _index >= 5 );
		return m_debugMouseButtonStates[ _index ];
	}
#endif

protected:
	enum class DriverInputEventType
	{
		UNUSUED = 0,

		GAMEPAD_JOYSTICK_UPDATE,
		GAMEPAD_BUTTON_DOWN,
		GAMEPAD_BUTTON_UP,

		KEY_DOWN,
		KEY_UP,

		MOUSE_DOWN,
		MOUSE_UP,
		MOUSE_MOVE,
		MOUSE_WHEEL
	};

	struct DriverInputEvent
	{
		DriverInputEventType eventType;

		// Mouse

		wv::Vector2f mouseMotion; // delta mouse motion
		wv::Vector2f mousePosition; // absolute mouse position

		int mouseButtonID; // mouse button index
	};

#ifndef WV_PACKAGE
	wv::Vector2f m_debugMouseMotion{ 0.0f, 0.0f };
	wv::Vector2f m_debugMousePosition{ 0.0f, 0.0f };
	bool m_debugMouseButtonStates[ 5 ] = { false, false, false, false, false };
#endif

	std::vector<DriverInputEvent> m_driverEvents;
};

class Application 
{
public:
	Application();
	
	static Application* getSingleton() { return Application::singleton; }

	bool initialize( World* _world, int _windowWidth, int _windowHeight );
	void shutdown();

	bool tick();

	double getApplicationTime( void ) const { return m_runtime; }
	double getDeltaTime      ( void ) const { return m_deltatime; }

	std::string getGraphicsDriverName() const { return m_graphicsDriverName; }
	
	void quit() { m_alive = false; }

	wv::InputSystem m_inputSystem;

private:
	
	static Application* singleton;

	void update();
	void render();

	std::string m_graphicsDriverName = "opengl";

	bool m_alive = true;

	uint64_t m_lastTicks = 0;
	double m_runtime = 0.0;
	double m_deltatime = 1.0 / 60.0;

	const double m_fixed_delta_time = 0.01;
	double m_fixed_runtime = 0.0;
	double m_accumulator = 0.0;

	// Subsystems

	wv::DisplayDriver* m_displayDriver = nullptr;
	wv::IFileSystem* m_filesystem = nullptr;
	wv::OpenGLRenderer m_renderer;

	wv::ResourceID m_material;
	
	wv::World* m_world = nullptr;
};

}
