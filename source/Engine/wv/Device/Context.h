#pragma once

#include <GLFW/glfw3.h>
#include <wv/Types.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

namespace wv
{

	struct ContextDesc
	{
		const char* name;
		int height;
		int width;

		wv::GraphicsAPI graphicsApi;
		wv::GenericVersion graphicsApiVersion;
	};

	class Context
	{
	public:
		Context( ContextDesc* _desc );

		void terminate();

		GraphicsDriverLoadProc getLoadProc();

		void pollEvents();
		void swapBuffers();

		void onResize( int _width, int _height );

		bool isAlive();

		int    getWidth()     { return m_width; }
		int    getHeight()    { return m_height; }
		double getTime()      { return m_time; };
		double getDeltaTime() { return m_frameTime; }
		float  getAspect();

		void setMouseLock( bool _lock );
	private:

		GLFWwindow* m_windowContext;
		int m_width = 0;
		int m_height = 0;
		const char* m_title = nullptr;

		double m_time = 0.0f;
		double m_frameTime = 0.016f;
	};
}