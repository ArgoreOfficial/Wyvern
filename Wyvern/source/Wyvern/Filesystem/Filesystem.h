#pragma once
#include <fstream>
#include <Wyvern/Logging/Logging.h>

namespace WV
{
	class Filesystem
	{
	public:
		static bool fileExists( std::string _path, bool _printError = false )
		{
			std::ifstream infile( _path );
			if ( _printError && !infile.good() )
			{
				WVERROR( "File not found:\n    %s", _path );
			}

			return infile.good();
		}
	};
}