#pragma once

namespace cm { class cRenderer; }
namespace cm { class cWindow; }

#include <wv/Core/iSingleton.h>
#include <wv/Math/Vector2.h>
#include <wv/Memory/Function.h>

#include <wv/Events/iInputListener.h>
#include <wv/Core/iApplicationConfig.h>

#include <stdio.h>

#define AVERAGE_FRAMETIME_BUFFER_SIZE 100

// TODO: change to proper event system
struct sMouseInfo
{
	wv::cVector2i mouse_position;
};

namespace wv
{
	class iCamera;

	class cApplication : public iSingleton<cApplication>, public iInputListener
	{
	public:
		
		 cApplication( void );
		~cApplication( void );

		void create    ( void ) override;
		void onResize  ( int _width, int _height );

		cm::cWindow* getWindow( void ) { return m_window; }
		
		void run( iApplicationConfig* _config_creator );

		virtual void onInputEvent( sInputEvent _info ) override;

		// TODO: move to scene
		iCamera* m_current_camera = nullptr;
		iCamera* m_camera2D       = nullptr;
		iCamera* m_camera3D       = nullptr;

	private:
		void init  ( void );
		void update( double _delta_time );
		void render( void );

		void updateDeltaTime( double& _time, double& _delta_time );

		double m_frametime_buffer[ AVERAGE_FRAMETIME_BUFFER_SIZE ];
		double m_average_frametime     = 0.0;
		int m_frametime_buffer_counter = 0;

		cm::cWindow* m_window;
	
		sApplicationConfig m_config;

		iApplicationConfig* m_config_creator;

	};
}

