#pragma once

namespace cm { class cRenderer; }
namespace cm { class cWindow; }
class iScene;

#include <wv/Core/iSingleton.h>

struct sInputInfo
{
	int key;
	int scancode;
	int mods;

	bool buttondown;
	bool buttonup;
	bool repeat;
};

namespace wv
{
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

		cm::cWindow* getWindow    ( void ) { return m_window; }
	
		void run( void );

	private:

		void updateDeltaTime( double& _time, double& _delta_time );

		cm::cWindow* m_window;
	
	};
}

