#pragma once

namespace cm { class cRenderer; }
namespace cm { class cWindow; }

#include <wv/Core/iSingleton.h>
#include <wv/Math/Vector2.h>

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
	class cCamera;

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
		cCamera* m_current_camera;
		cCamera* m_camera2D;
		cCamera* m_camera3D;

	private:

		void updateDeltaTime( double& _time, double& _delta_time );

		cm::cWindow* m_window;
	
	};
}

