#pragma once

#include <wv/math/vector3.h>
#include <wv/renderer.h>
#include <wv/camera/camera.h>

struct GLFWwindow;

namespace wv {

struct VertexData
{
	wv::Vector3f position;
};

class Application 
{
public:
	Application() = default;
	
	bool initialize();
	bool tick();
	void shutdown();

	double getApplicationTime( void ) const { return m_time; }
	double getDeltaTime      ( void ) const { return m_deltatime; }
private:
	
	void update();
	void render();

	GLFWwindow* m_window;
	double m_time = 0.0;
	double m_deltatime = 0.0;

	int m_windowWidth;
	int m_windowHeight;

	wv::OpenGLRenderer m_renderer;

	wv::GLShaderPipeline shader;
	wv::GLVertexBuffer vbo;

	wv::ICamera* m_camera = nullptr;
};

}
