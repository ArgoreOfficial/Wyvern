#include "command.h"
#include "wv/debug/log.h"

void wv::ConsoleCommand::add( const std::string& _arg )
{
	mStrings.push_back( _arg );
}

void wv::ConsoleCommand::add( const std::string& _arg, const std::string& _value )
{
	mStrings.push_back( _arg + " " + _value );
}

int wv::ConsoleCommand::run( const std::string& _runDir )
{
	// construct command
	std::string cmd = "cd \"" + _runDir + "\" && ";
	
	int i = 0;
	for ( std::string& str : mStrings )
	{
		if( i == 0 )
			cmd.append( "\"" + str + "\" "); // first one, run with " "
		else
			cmd.append( str + " " );
		i++;
	}
	
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "%s\n", cmd.c_str());
	return std::system( cmd.c_str() );
}

int wv::ConsoleCommand::runf( const std::string& _runDir, std::vector<std::string> _l )
{
	wv::ConsoleCommand cmd{ _l };
	return cmd.run( _runDir );
}
