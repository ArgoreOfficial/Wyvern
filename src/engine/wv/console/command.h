#pragma once

#include <string>
#include <vector>
#include <initializer_list>

namespace wv {

class ConsoleCommand
{
public:
	ConsoleCommand() = default;
	ConsoleCommand( std::initializer_list<std::string> _l ) : 
		mStrings{ _l } 
	{ }

	void add( const std::string& _arg );
	void add( const std::string& _arg, const std::string& _value );

	int run( const std::string& _runDir = "./" );

private:

	std::string mRunEnvironment;
	std::vector<std::string> mStrings;

};

}