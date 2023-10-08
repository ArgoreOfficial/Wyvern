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

	private:
		GLFWwindow* m_window;
	};
}