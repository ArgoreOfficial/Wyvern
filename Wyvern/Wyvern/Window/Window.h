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
		void setTitle( const char* _title ) { glfwSetWindowTitle( m_window, _title ); }
		
		void touch() { bgfx::touch( m_clearView ); }
		void shutdown() { glfwTerminate(); }

	private:
		GLFWwindow* m_window;
		bgfx::ViewId m_clearView = 0;
	};
}
