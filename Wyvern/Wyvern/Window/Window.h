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
		int createWindow( int _width, int _height, const char* _title );
		int pollEvents();
		void processInput();
		void hookEvents();

		GLFWwindow* getWindow() { return m_window; }
		bgfx::ViewId& getView() { return m_clearView; }
		void setTitle( const char* _title ) { glfwSetWindowTitle( m_window, _title ); }
		void setVSync( bool _value );
		
		int getWidth() { return m_view_width; }
		int getHeight() { return m_view_height; }
		float getAspect() { return float( m_view_width ) / float( m_view_height ); }

		void touch() { bgfx::touch( m_clearView ); }
		inline void shutdown() 
		{ 
			WVDEBUG("Terminated GLFW");
			glfwTerminate(); 
		}

		void windowResizeCallback( GLFWwindow* _window, int _width, int _height );

	private:
		GLFWwindow* m_window;
		bgfx::ViewId m_clearView = 0;
		
		bool m_vsync_enabled = false;
		int m_view_width;
		int m_view_height;

		static void handleApplicationEvents();
		static void handleKeyEvents( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void handleMouseEvents( GLFWwindow* window, double xpos, double ypos );
	};
}
