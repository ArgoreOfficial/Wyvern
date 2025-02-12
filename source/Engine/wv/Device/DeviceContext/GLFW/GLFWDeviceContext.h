#pragma once

#ifdef WV_SUPPORT_GLFW
#include <GLFW/glfw3.h>
#endif

#include <wv/Types.h>

#include <wv/Device/DeviceContext.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class GLFWDeviceContext : public iDeviceContext
	{

	public:
		GLFWDeviceContext() {}

		void terminate() override;

		virtual void initImGui() override;
		virtual void terminateImGui() override;
		virtual bool newImGuiFrame() override;
		virtual void renderImGui() override;

		GraphicsDriverLoadProc getLoadProc() override;

		void pollEvents() override;
		void swapBuffers() override;

		void onResize( int _width, int _height ) override;
		void setSize( int _width, int _height ) override;

		void setMouseLock( bool _lock ) override;
		void setTitle( const char* _title ) override;

		void setSwapInterval( int _interval ) override;

///////////////////////////////////////////////////////////////////////////////////////

	#ifdef WV_SUPPORT_GLFW
		GLFWwindow* m_windowContext = nullptr;
	#endif

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		friend class iDeviceContext;
		
		bool initialize( ContextDesc* _desc ) override;

	};
}