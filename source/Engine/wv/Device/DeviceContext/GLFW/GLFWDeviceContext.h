#pragma once

#ifdef WV_GLFW_SUPPORTED
#include <GLFW/glfw3.h>
#endif

#include <wv/Types.h>

#include <wv/Device/DeviceContext.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class GLFWDeviceContext : public iDeviceContext
	{

	public:

		void terminate() override;

		GraphicsDriverLoadProc getLoadProc() override;

		void pollEvents() override;
		void swapBuffers() override;

		void onResize( int _width, int _height ) override;
		void setSize( int _width, int _height ) override;

		void setMouseLock( bool _lock ) override;
		void setTitle( const char* _title ) override;

		void setSwapInterval( int _interval ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		friend class iDeviceContext;
		GLFWDeviceContext( ContextDesc* _desc );
		
	#ifdef WV_GLFW_SUPPORTED
		GLFWwindow* m_windowContext = nullptr;
	#endif

	};
}