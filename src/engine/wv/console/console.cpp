#include "console.h"

#include <wv/debug/log.h>

bool wv::Console::Internal::g_initialized = false;

bool wv::Console::isInitialized()
{
	return Internal::g_initialized;
}

int wv::Console::run( const std::string& _runDir, const std::vector<std::string>& _cmd )
{
	// construct command
	std::string cmd = "cd \"" + _runDir + "\" && ";

	int i = 0;
	for ( std::string str : _cmd )
	{
		if ( i == 0 )
			cmd.append( "\"" + str + "\" " ); // first one, run with " "
		else
			cmd.append( str + " " );
		i++;
	}

	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "%s\n", cmd.c_str() );
	return std::system( cmd.c_str() );
}
