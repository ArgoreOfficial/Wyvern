#pragma once
#include <Wyvern/Logging/cLogging.h>
#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace WV
{
	class cWindow
	{
	public:
		cWindow();
		~cWindow();
		int createWindow( const char* _title );
		int createWindow( int _width, int _height, const char* _title );
		int pollEvents();
		void processInput();
		void hookEvents();

		void setTitle( const char* _title ) { glfwSetWindowTitle( m_window, _title ); }
		void setVSync( bool _value );
		void setClearColour( uint32_t hex );

		GLFWwindow* getWindow() { return m_window; }
		int getWidth() { return m_view_width; }
		int getHeight() { return m_view_height; }
		float getAspect() { return float( m_view_width ) / float( m_view_height ); }

		inline void shutdown() 
		{
			WV_DEBUG("Terminated GLFW");
			glfwTerminate(); 
		}

		void windowResizeCallback( GLFWwindow* _window, int _width, int _height );

	private:
		GLFWwindow* m_window = nullptr;
		bool m_vsync_enabled = false;
		int m_view_width     = 0;
		int m_view_height    = 0;

		static void handleApplicationEvents();
		static void handleKeyEvents( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void handleMouseEvents( GLFWwindow* window, double xpos, double ypos );
	};
}
