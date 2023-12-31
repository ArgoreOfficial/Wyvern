#pragma once

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iLayer
	{

	public:

		 iLayer( std::string _debugName = "Layer" ) : m_debugName( _debugName ) { }
		~iLayer() { }

///////////////////////////////////////////////////////////////////////////////////////

		virtual void start ( void ) { }
		virtual void update( double _deltaTime ) { }
		virtual void draw3D( void ) { }
		virtual void draw2D( void ) { }
		virtual void drawUI( void ) { }

///////////////////////////////////////////////////////////////////////////////////////

	private:
		std::string m_debugName = "Layer";
		bool m_enabled = true;

	};

///////////////////////////////////////////////////////////////////////////////////////

}
