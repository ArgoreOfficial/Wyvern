#pragma once
#include <Wyvern/Logging/Logging.h>
#include <GL/glew.h>
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
		void hookEvents();

	private:
		GLFWwindow* m_window;

		static void handleApplicationEvents( );
		static void handleKeyEvents( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void handleMouseEvents( GLFWwindow* window, double xpos, double ypos );
	};
}
