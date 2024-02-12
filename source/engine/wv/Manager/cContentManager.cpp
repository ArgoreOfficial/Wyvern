#include "cContentManager.h"

#include <iostream>
#include <fstream>

void wv::cContentManager::create()
{
}

std::string wv::cContentManager::loadShaderSource( const std::string& _path )
{
	std::string line, text;
	std::ifstream in( _path );

	while ( std::getline( in, line ) )
	{
		text += line + "\n";
	}

	return text;
}
