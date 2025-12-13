#pragma once

#include <wv/math/vector3.h>
#include <wv/graphics/renderer.h>
#include <wv/camera/camera.h>

namespace wv {

class IFileSystem;
class DisplayDriver;

struct VertexData
{
	wv::Vector3f normal;
	wv::Vector3f color;
	wv::Vector2f texCoord;
};

struct Scene
{
	std::vector<ResourceID> models; // TODO: proper object node
	std::vector<wv::ICamera*> cameras;
	uint16_t activeCameraIndex = 0;

	wv::ICamera* getActiveCamera() { 
		if ( activeCameraIndex >= cameras.size() )
			return nullptr;

		return cameras[ activeCameraIndex ]; 
	}

	// std::vector<Light> lights;
	// etc...
};

class Application 
{
public:
	Application();
	
	static Application* getSingleton() { return Application::singleton; }

	bool initialize( int _windowWidth, int _windowHeight );
	bool tick();
	void shutdown();

	double getApplicationTime( void ) const { return m_runtime; }
	double getDeltaTime      ( void ) const { return m_deltatime; }

	std::string getGraphicsDriverName() const { return m_graphicsDriverName; }
	wv::Scene* getActiveScene() {
		if ( m_activeSceneIndex >= m_scenes.size() )
			return nullptr;

		return m_scenes[ m_activeSceneIndex ];
	}

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
	
	std::vector<Scene*> m_scenes;
	size_t m_activeSceneIndex = 0;

	wv::RenderView m_renderView; // TODO?

};

}
