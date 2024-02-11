#pragma once

namespace cm { class cRenderer; }
namespace cm { class cWindow; }
class iScene;

#include <wv/Core/iSingleton.h>
#include <string>

struct sInputInfo
{
	int key;
	int scancode;
	int mods;

	bool buttondown;
	bool buttonup;
	bool repeat;
};

class cApplication : public iSingleton<cApplication>
{
public:
	enum class eBackend
	{
		kGL3,
		kD3D11
	};

	 cApplication( void );
	~cApplication( void );

	void onCreate  ( void ) override;
	void onResize  ( int _width, int _height );
	void onRawInput( sInputInfo* _info );

	cm::cWindow* getWindow    ( void ) { return m_window; }
	cm::cRenderer* getRenderer( void ) { return m_renderer; }

	std::string loadShaderSource( const char* _path );

	void run( void );

private:

	cm::cRenderer* m_renderer;

	cm::cWindow* m_window;
	
};
