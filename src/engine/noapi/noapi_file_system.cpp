#include "noapi_file_system.h"

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIFileSystem::initialize()
{}

wv::FileID wv::NoAPIFileSystem::openFile( const char* _path, const OpenMode& _mode )
{
	FILE* f = nullptr;

	switch( _mode )
	{
	case WV_OPEN_MODE_READ:      f = fopen( _path, "rb" ); break;
	case WV_OPEN_MODE_READWRITE: f = fopen( _path, "r+" ); break;
	case WV_OPEN_MODE_WRITE:     f = fopen( _path, "w"  ); break;
	}

	if( f == nullptr )
		return FileID::InvalidID;

	return FileID( reinterpret_cast<uint64_t>( f ) );
}

///////////////////////////////////////////////////////////////////////////////////////

uint64_t wv::NoAPIFileSystem::getFileSize( FileID& _file )
{
	FILE* f = reinterpret_cast<FILE*>( _file.value );

	fseek( f, 0L, SEEK_END );
	size_t size = ftell( f );
	rewind( f );

	return size;
}

///////////////////////////////////////////////////////////////////////////////////////

int wv::NoAPIFileSystem::readFile( FileID& _file, uint8_t* _buffer, const size_t& _size )
{
	FILE* f = reinterpret_cast<FILE*>( _file.value );
	size_t size = fread( _buffer, 1, _size, f );
	return size;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIFileSystem::writeFile( FileID& _file, uint8_t* _buffer, const size_t& _size )
{
	FILE* f = reinterpret_cast<FILE*>( _file.value );

	fwrite( _buffer, 1, _size, f );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIFileSystem::closeFile( FileID& _file )
{
	FILE* f = reinterpret_cast<FILE*>( _file.value );

	fclose( f );
}

