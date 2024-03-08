#pragma once

#include <stdint.h>
#include <Windows.h>

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace cm
{


	class cWindow
	{
	public:
		 cWindow( void );
		~cWindow( void );

		void create ( void );
		void update ( void );
		void destroy( void );

		void createSurface( VkInstance _instance, VkSurfaceKHR* _surface);

		uint32_t getWidth ( void ) const { return m_width; }
		uint32_t getHeight( void ) const { return m_height; }
		
		HWND         getWin32Window               ( void );
		const char** getRequiredInstanceExtensions( uint32_t* _count );
		void         getFramebufferSize           ( int* _out_width, int* _out_height );

		bool shouldClose( void );
		void waitEvents( void );

	private:

		GLFWwindow* m_window;

		uint32_t m_width  = 512;
		uint32_t m_height = 512;

		double m_last_time = 0.0;

	};
}