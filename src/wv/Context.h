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

		void beginFrame();
		void endFrame();

		bool isAlive();

	private:

		GLFWwindow* m_windowContext;
	};
}