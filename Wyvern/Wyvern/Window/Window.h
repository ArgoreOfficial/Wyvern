#pragma once
#include <Wyvern/Logging/Logging.h>
#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>

namespace WV
{
	class Window
	{
	public:
		Window();
		~Window();
		int createWindow();
		int pollEvents();
		void processInput();

		GLFWwindow* getWindow() { return m_window; }
		bgfx::ViewId& getView() { return m_clearView; }
		void setTitle( const char* _title ) { glfwSetWindowTitle( m_window, _title ); }
		
		void touch() { bgfx::touch( m_clearView ); }
		inline void shutdown() 
		{ 
			WVDEBUG("Terminated GLFW");
			glfwTerminate(); 
		}

	private:
		GLFWwindow* m_window;
		bgfx::ViewId m_clearView = 0;
	};
}
