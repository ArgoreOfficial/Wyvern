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

		virtual void debugInit  ( void ) = 0; 
		virtual void debugUpdate( double _delta_time ) = 0;
		virtual void debugRender( void ) = 0;

	protected:
		
	};
}
