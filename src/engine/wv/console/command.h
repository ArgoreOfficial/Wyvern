#pragma once

#include <string>
#include <vector>
#include <initializer_list>

namespace wv {

class ConsoleCommand
{
public:
	ConsoleCommand() = default;
	ConsoleCommand( const std::vector<std::string>& _v )    : mStrings{ _v } { }
	ConsoleCommand( std::initializer_list<std::string> _l ) : mStrings{ _l } { }

	void add( const std::string& _arg );
	void add( const std::string& _arg, const std::string& _value );

	int run( const std::string& _runDir = "./" );
	static int runf( const std::string& _runDir = "./", std::vector<std::string> _l = { } );

private:

	std::string mRunEnvironment;
	std::vector<std::string> mStrings;

};

}