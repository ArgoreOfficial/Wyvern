#pragma once

#include <wv/Types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	enum DeviceContextAPI
	{
		WV_DEVICE_CONTEXT_API_SDL,
		WV_DEVICE_CONTEXT_API_GLFW
	};

///////////////////////////////////////////////////////////////////////////////////////

	enum eVSyncMode
	{
		WV_VSYNC_OFF,
		WV_VSYNC_ON,
		WV_VSYNC_ADAPTIVE
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct ContextDesc
	{
		const char* name = "";
		int height = 480;
		int width = 640;
		bool allowResize = false;

		DeviceContextAPI deviceApi;
		GraphicsAPI graphicsApi;
		GenericVersion graphicsApiVersion;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class iGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext
	{

	public:

		friend class iGraphicsDevice;

		static iDeviceContext* getDeviceContext( ContextDesc* _desc );

		virtual void terminate() = 0;
		virtual GraphicsDriverLoadProc getLoadProc() = 0;

		virtual void pollEvents () = 0;
		virtual void swapBuffers() = 0;

		virtual void onResize( int _width, int _height ) { setSize( _width, _height ); }
		virtual void setSize( int _width, int _height );

		virtual void setMouseLock( bool _lock ) = 0;
		virtual void setTitle( const char* _title ) = 0;

		virtual void setSwapInterval( int _interval ) = 0;

		DeviceContextAPI getContextAPI     ( void ) { return m_deviceApi; }
		GraphicsAPI      getGraphicsAPI    ( void ) { return m_graphicsApi; }
		GenericVersion   getGraphicsVersion( void ) { return m_graphicsApiVersion; }

///////////////////////////////////////////////////////////////////////////////////////
		
		int    getWidth    () { return m_width; }
		int    getHeight   () { return m_height; }
		double getTime     () { return m_time; };
		double getDeltaTime() { return m_deltaTime; }
		bool   isAlive     () { return m_alive; };

		void close() { m_alive = false; }

		inline float getAspect()
		{
			if ( m_width == 0 || m_height == 0 )
				return 1.0f;

			return (float)m_width / (float)m_height;
		}

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		virtual bool initialize( ContextDesc* _desc ) = 0;

		DeviceContextAPI m_deviceApi;
		GraphicsAPI      m_graphicsApi;
		GenericVersion   m_graphicsApiVersion;

		int m_width = 0;
		int m_height = 0;
		const char* m_title = nullptr;

		double m_time = 0.0f;
		double m_deltaTime = 0.016f;

		bool m_alive = true;

	};

}