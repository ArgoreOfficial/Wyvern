#pragma once

#include <wv/Math/Vector4.h>

// TODO: merge cm into wv?
namespace cm
{
	class cWindow;
	
	enum eDrawMode
	{
		DrawMode_Points,
		DrawMode_Lines,
		DrawMode_LineStrip,
		DrawMode_LineLoop,
		DrawMode_Triangle
	};

	enum eClearMode
	{
		ClearMode_Color   = 0b001,
		ClearMode_Depth   = 0b010,
		ClearMode_Stencil = 0b100
	};

	class iBackend
	{
	public:
		virtual ~iBackend() { }

		// TODO: change cWindow* to sApplicationState* 
		virtual void create ( cWindow* _window ) = 0;
		virtual void destroy( void ) = 0;

		virtual void onResize( int _width, int _height ) = 0;

		virtual void beginFrame( void ) = 0;
		virtual void drawFrame ( void ) = 0; // TODO: add sDrawPacket
		virtual void endFrame  ( void ) = 0;
		
	protected:
		iBackend() { }

	private:

	};
}
