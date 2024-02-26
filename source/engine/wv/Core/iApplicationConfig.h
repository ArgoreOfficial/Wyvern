#pragma once

#include <wv/Decl.h>
#include <string>

namespace wv
{

	struct sApplicationConfig
	{
		std::string name;
		std::string title;
		std::string version;

	};

	class iApplicationConfig
	{
	WV_DECLARE_INTERFACE( iApplicationConfig )

	public:
		virtual sApplicationConfig config( void ) = 0;

		virtual void debugInit  ( void )               { }; 
		virtual void debugUpdate( double _delta_time ) { };
		virtual void debugRender( void )               { };

		virtual void debugViewbufferBegin( void ) { };
		virtual void debugViewbufferEnd  ( void ) { };

	protected:
		
	};
}
