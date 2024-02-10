#pragma once

class cRenderer;
class cWindow;
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

	void onCreate() override;
	void onResize( int _width, int _height );
	void onRawInput( sInputInfo* _info );

	cWindow* getWindow() { return m_window; }
	void run    ( void );
	cRenderer* getRenderer() { return m_renderer; }

	std::string loadShaderSource( const char* _path );

private:

	cRenderer* m_renderer;

	cWindow* m_window;
	
};
