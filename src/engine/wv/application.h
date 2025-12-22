#pragma once

#include <wv/math/vector3.h>
#include <wv/graphics/renderer.h>
#include <wv/camera/camera.h>

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
	
	size_t m_activeSceneIndex = 0;

	wv::RenderView m_renderView; // TODO?

	wv::World* m_world = nullptr;
	std::vector<ResourceID> models;
};

}
