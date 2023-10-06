#pragma once
#include <Wyvern/API/Core.h>
#include <Wyvern/Log/Log.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace WV
{
	class WYVERN_API Window
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
