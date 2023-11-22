#pragma once

#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Misc/cColor.h>
#include <Wyvern/Renderer/Camera/cCamera.h>

#include <string>

#if defined( WV_PLATFORM_WINDOWS ) || defined( WV_PLATFORM_WINDOWS_32 ) || defined( WV_PLATFORM_LINUX ) || defined( WV_PLATFORM_MAC )

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"

#endif

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cViewport
	{

	public:

		enum class eViewportState
		{
			kOpen,
			kFocused,
			kMinimized,
			kClosing
		};

		 cViewport();
		~cViewport();

///////////////////////////////////////////////////////////////////////////////////////

		void create       ( std::string _title, unsigned int _width, unsigned int _height );
		void destroy      ( void );
		void initImguiImpl( void );
		void hookEvents   ( void );

///////////////////////////////////////////////////////////////////////////////////////

		void setTitle       ( const std::string& _title );
		void setVSync       ( bool _value );
		void setFullscreen  ( bool _value );
		void setClearColor  ( cColor _color )    { m_clearColor = _color; }
		void setActiveCamera( cCamera* _camera ) { m_activeCamera = _camera; }

///////////////////////////////////////////////////////////////////////////////////////

		unsigned int   getWidth ( void ) { return m_viewWidth; }
		unsigned int   getHeight( void ) { return m_viewHeight; }
		eViewportState getState ( void ) { return m_state; }

///////////////////////////////////////////////////////////////////////////////////////

		void processInput( void );
		void update      ( void );
		void clear       ( const cColor& _color );
		void display     ( void );
		void clear       ( void ) { clear( m_clearColor ); }

///////////////////////////////////////////////////////////////////////////////////////

	private:

	#if defined( WV_PLATFORM_WINDOWS ) || defined( WV_PLATFORM_WINDOWS_32 ) || defined( WV_PLATFORM_LINUX ) || defined( WV_PLATFORM_MAC )
		GLFWwindow* m_window = nullptr;
	#endif
		
		eViewportState m_state  = eViewportState::kOpen;
		cCamera* m_activeCamera = nullptr;
		cColor m_clearColor     = Color::PacificBlue;
		int m_viewWidth         = 0;
		int m_viewHeight        = 0;

	};

///////////////////////////////////////////////////////////////////////////////////////

}