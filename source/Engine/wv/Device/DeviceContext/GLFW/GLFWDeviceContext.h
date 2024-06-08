#pragma once

#include <GLFW/glfw3.h>
#include <wv/Types.h>

#include <wv/Device/DeviceContext.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class GLFWDeviceContext : public DeviceContext
	{

	public:

		void terminate();

		GraphicsDriverLoadProc getLoadProc();

		void pollEvents();
		void swapBuffers();

		void onResize( int _width, int _height );

		bool isAlive();

		void setMouseLock( bool _lock );
		void setTitle( const char* _title );

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		friend class DeviceContext;
		GLFWDeviceContext( ContextDesc* _desc );
		
		GLFWwindow* m_windowContext;

	};
}