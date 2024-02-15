#pragma once

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <wv/Math/Vector2.h>

namespace cm
{
	enum eWindowAttribute
	{
		WindowAttribute_Decorated = 0,
		WindowAttribute_Fullscreen,
		WindowAttribute_ClickThrough,
		WindowAttribute_Transparent,
		WindowAttribute_Focused,
		WindowAttribute_FocusOnShow,
		WindowAttribute_AlwaysOnTop,
		WindowAttribute_Resizable,
		WindowAttribute_Maximized,
		WindowAttribute_Minimized,
		eWindowAttribute_SIZE
	};

	class cWindow
	{
	public:
		 cWindow( void );
		~cWindow( void );

		int  create ( unsigned int _width, unsigned int _height, const char* _title );
		void destroy( void );

		void processInput( void );
		void display  ( void );

		void onResize( int _width, int _height );

		GLFWwindow* getWindowObject( void ) { return m_window_object; }

		unsigned int getWidth   ( void ) { return m_width; }
		unsigned int getHeight  ( void ) { return m_height; }
		double       getTime    ( void ) { return glfwGetTime(); }
		bool         shouldClose( void ) { return glfwWindowShouldClose( m_window_object ); } // TODO: change to state enum
		float        getAspect  ( void );

		wv::cVector2i getMousePosition( void ) { return m_mouse_pos; }
		
		void setTitle( const char* _title ) { m_title = _title; glfwSetWindowTitle( m_window_object, _title ); }
		void setVSync( bool _state )        { glfwSwapInterval( _state ); }

		void setWindowAttribute( eWindowAttribute _attribute, int _value );
		void applyWindowAttributes( bool _recreate_window = true );
	private:

		wv::cVector2i m_mouse_pos;
		wv::cVector2i m_prev_pos;

		void createWindow();

		GLFWwindow* m_window_object = nullptr;
		unsigned int m_width = 0;
		unsigned int m_height = 0;
		const char* m_title = nullptr;
		int m_window_attributes[ eWindowAttribute_SIZE ] =
		{
			true,  // WindowAttribute_Decorated
			false, // WindowAttribute_Fullscreen
			false, // WindowAttribute_ClickThrough
			false, // WindowAttribute_Transparent
			false, // WindowAttribute_Focused
			false, // WindowAttribute_FocusOnShow
			false, // WindowAttribute_AlwaysOnTop
			true,  // WindowAttribute_Resizable
			false, // WindowAttribute_Maximized
			false, // WindowAttribute_Minimized
		};

	};
}

