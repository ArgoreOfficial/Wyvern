#pragma once
#include <string>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Misc/cColor.h>
#include <Wyvern/Renderer/Camera/cCamera.h>

#if defined( WV_PLATFORM_WINDOWS ) || defined( WV_PLATFORM_WINDOWS_32 )

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"

#endif


namespace wv
{
	class cViewport
	{

	public:

		cViewport();
		~cViewport();

		void create( std::string _title, unsigned int _width, unsigned int _height );
		void destroy();
		void initImguiImpl();
		void hookEvents();

		void setTitle( const std::string& _title );
		void setActiveCamera( cCamera* _camera ) { m_activeCamera = _camera; }
		void setVSync( bool _value );
		void setClearColor( cColor _color ) { m_clearColor = _color; }

		unsigned int getWidth() { return m_viewWidth; }
		unsigned int getHeight() { return m_viewHeight; }
		bool shouldClose() { return m_shouldClose; }

		void processInput();
		void update();
		void clear() { clear( m_clearColor ); }
		void clear( const cColor& _color );
		void display( void );
		
	private:

	#if defined( WV_PLATFORM_WINDOWS ) || defined( WV_PLATFORM_WINDOWS_32 ) 
		GLFWwindow* m_window = nullptr;
	#endif
		
		cCamera* m_activeCamera = nullptr;
		cColor m_clearColor     = Color::PacificBlue;

		int m_viewWidth         = 0;
		int m_viewHeight        = 0;
		
		bool m_shouldClose      = false;
	};
}