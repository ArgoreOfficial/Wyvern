#pragma once

#include <wv/math/vector3.h>
#include <wv/graphics/renderer.h>
#include <wv/camera/camera.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

struct SDL_Window;
typedef void* SDL_GLContext;

namespace wv {

struct VertexData
{
	wv::Vector3f position;
};

class Application 
{
public:
	Application() = default;
	
	bool initialize( int _windowWidth, int _windowHeight );
	bool tick();
	void shutdown();

	double getApplicationTime( void ) const { return m_runtime; }
	double getDeltaTime      ( void ) const { return m_deltatime; }
private:
	
	void update();
	void render();

#ifdef WV_SUPPORT_SDL2
	SDL_Window* m_window_context = nullptr;
#ifdef WV_SUPPORT_OPENGL 
	SDL_GLContext m_opengl_context = nullptr;
#endif
#endif

	bool m_alive = true;

	uint64_t m_performance_counter = 0;
	double m_runtime = 0.0f;
	double m_deltatime = 1.0 / 60.0;

	const double m_fixed_delta_time = 0.01;
	double m_fixed_runtime = 0.0;
	double m_accumulator = 0.0;

	int m_windowWidth = 0;
	int m_windowHeight = 0;

	wv::OpenGLRenderer m_renderer;

	wv::GLShaderPipeline shader;
	wv::GLVertexBuffer vbo;

	wv::ICamera* m_camera = nullptr;
};

}
