#include "cFilesystem.h"

#include <fstream>
#include <sstream>
#include <filesystem>

#include <Wyvern/Logging/cLogging.h>

using namespace wv;

///////////////////////////////////////////////////////////////////////////////////////

bool Filesystem::fileExists( std::string _path, bool _printError )
{

	std::ifstream infile( _path );
	if ( _printError && !infile.good() )
	{
		WV_ERROR( "File not found:\n    %s", _path.c_str() );
	}

	return infile.good();

}

///////////////////////////////////////////////////////////////////////////////////////

std::string Filesystem::getFilenameFromPath( std::string _path, bool _keepExtension )
{

	std::string filename = _path.substr( _path.find_last_of( "/\\" ) + 1 );

	if ( !_keepExtension )
	{
		std::string::size_type const p( filename.find_last_of( '.' ) );
		filename = filename.substr( 0, p );
	}

	return filename.c_str();

}

///////////////////////////////////////////////////////////////////////////////////////

std::string Filesystem::getFileExtension( std::string _path )
{

	std::string::size_type const p( _path.find_last_of( '.' ) );
	std::string extension = _path.substr( p, _path.size() );
	
	return extension;

}

///////////////////////////////////////////////////////////////////////////////////////

std::vector<char>* Filesystem::loadByteArrayFromPath( std::string _path )
{

	std::ifstream stream( _path, std::ios::binary );

	std::vector<char>* data = new std::vector<char>( std::filesystem::file_size( _path ) / sizeof( char ) );
	stream.read( reinterpret_cast<char*>( data->data() ), data->size() * sizeof( char ) );

	return data;

}

	/*
const bgfx::Memory* wv::Filesystem::loadMemoryFromFile( std::string _path )
{
	std::vector<char>* data = loadByteArrayFromPath( _path );

	uint32_t size = data->size();

	const bgfx::Memory* mem = bgfx::alloc( size + 1 );
	memcpy( mem->data, data->data(), size);

	mem->data[ mem->size - 1 ] = '\0';
	return mem;
}
	*/
