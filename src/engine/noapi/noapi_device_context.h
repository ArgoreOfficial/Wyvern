#pragma once

#include <wv/types.h>
#include <wv/device/device_context.h>

#include <chrono>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class NoAPIDeviceContext : public IDeviceContext
	{

	public:
		NoAPIDeviceContext() {}

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

	protected:
		friend class IDeviceContext;
		
		bool initialize( ContextDesc* _desc ) override;

		bool m_isFirstFrame = true;
	#ifdef WV_PLATFORM_WINDOWS
		std::chrono::high_resolution_clock m_timer;
		std::chrono::steady_clock::time_point m_timepoint;
	#endif
	};
}