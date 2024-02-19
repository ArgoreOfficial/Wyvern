#pragma once

namespace cm { class cRenderer; }
namespace cm { class cWindow; }

#include <wv/Core/iSingleton.h>
#include <wv/Math/Vector2.h>

#define AVERAGE_FRAMETIME_BUFFER_SIZE 1000

// TODO: change to proper event system
struct sInputInfo
{
	enum
	{
		InputInfo_Key,
		InputInfo_Mouse
	} type ;

	int key;
	int scancode;
	int mods;

	bool buttondown;
	bool buttonup;
	bool repeat;

	wv::cVector2i mouse_position;
};

namespace wv
{
	class cSceneLoader;
	class iCamera;

	class cApplication : public iSingleton<cApplication> /* add singleton manager? */
	{
	public:
		enum class eBackend
		{
			kGL3,
			kD3D11
		};

		 cApplication( void );
		~cApplication( void );

		void create    ( void ) override;
		void onResize  ( int _width, int _height );
		
		void onRawInput( sInputInfo* _info );

		cm::cWindow* getWindow( void ) { return m_window; }
	
		void run( cSceneLoader* _scene_loader );

		// TODO: move to scene
		iCamera* m_current_camera = nullptr;
		iCamera* m_camera2D       = nullptr;
		iCamera* m_camera3D       = nullptr;

	private:

		void updateDeltaTime( double& _time, double& _delta_time );

		double m_frametime_buffer[ AVERAGE_FRAMETIME_BUFFER_SIZE ];
		double m_average_frametime     = 0.0;
		int m_frametime_buffer_counter = 0;

		cm::cWindow* m_window;
	
	};
}

