#pragma once
#include <fstream>
#include <sstream>
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
				WVERROR( "File not found:\n    %s", _path.c_str() );
			}

			return infile.good();
		}

		static std::string getFilenameFromPath( std::string _path )
		{
			std::string filename = _path.substr( _path.find_last_of( "/\\" ) + 1 );
			// std::string::size_type const p( filename.find_last_of( '.' ) );
			// std::string withoutExtension = filename.substr( 0, p );
			
			return filename.c_str();
		}
	};
}