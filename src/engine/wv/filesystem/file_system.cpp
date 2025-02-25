#include "file_system.h"

#include <wv/debug/log.h>
#include <wv/engine.h>
#include <wv/graphics/graphics_device.h>
#include <wv/graphics/Uniform.h>
#include <wv/memory/memory.h>
#include <wv/platform/platform.h>

#ifdef WV_PLATFORM_WINDOWS
#define STB_IMAGE_IMPLEMENTATION
#include <auxiliary/stb_image.h>
#endif

#include <fstream>
#include <vector>
#include <string>

#ifdef WV_CPP20
#include <locale>
#include <codecvt>
#include <filesystem>
#endif

#if defined( WV_PLATFORM_3DS )
#include <unistd.h>
#include <3ds.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

wv::IFileSystem::IFileSystem()
{
	// todo: fix
#ifdef WV_PLATFORM_3DS
	Result rc = romfsMountSelf( "data" );
	if( rc )
		printf( "romfsInit: %08lX\n", rc );
	else
	{
		printf( "romfs Init Successful!\n" );
	}
#endif
	
	addDirectory( "" );
}

wv::IFileSystem::~IFileSystem()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_PLATFORM_PSVITA
#define ERROR_PREFIX "Error:"
#endif

wv::Memory* wv::IFileSystem::loadMemory( const std::string& _path )
{
	std::string path = getFullPath( _path );
	if( path == "" )
		return nullptr;

	FileID file = openFile( path.c_str(), wv::OpenMode::WV_OPEN_MODE_READ );
	if( !file.is_valid() )
		return nullptr;

	uint64_t size = getFileSize( file );

	Memory* mem = WV_NEW( Memory );
	mem->data = WV_NEW_ARR( unsigned char, size );
	mem->size = static_cast<unsigned int>( size );

	readFile( file, mem->data, size );
	closeFile( file );

	Debug::Print( "Loaded file '%s'\n", path.c_str() );

#ifdef WV_PLATFORM_PSVITA
	if( strncmp( mem->data, ERROR_PREFIX, strlen( ERROR_PREFIX ) ) == 0 )
	{
		printf( "%s\n", mem->data );
		return nullptr;
	}
#endif

	return mem;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IFileSystem::unloadMemory( Memory* _memory )
{
	if ( !_memory )
		return;

	WV_FREE_ARR( _memory->data );
	*_memory = {};
	WV_FREE( _memory );
}

///////////////////////////////////////////////////////////////////////////////////////

std::string wv::IFileSystem::loadString( const std::string& _path )
{
	Memory* mem = loadMemory( _path );

	if ( !mem )
		return "";

	std::string str( (const char*)mem->data, mem->size );
	unloadMemory( mem );
    return str;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::IFileSystem::fileExists( const std::string& _path )
{
#ifdef WV_PLATFORM_WINDOWS
	std::ifstream f( _path );
	return f.good();
#else
	FILE* f = fopen( _path.c_str(), "r");
	if( f )
	{
		fclose( f );
		return true;
	}

	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

std::string wv::IFileSystem::getFullPath( const std::string& _fileName )
{
	if( fileExists( _fileName ) )
	{
		Debug::Print( "L:%s\n", _fileName.c_str() );
		return _fileName;
	}
	
	for ( size_t i = 0; i < m_directories.size(); i++ )
	{
		std::string path = "";
		path.append( gFileSystemPathPrefix ); // defined in <target platform>.cpp
		path.append( m_directories[ i ] );
		path.append( _fileName );

		if( fileExists( path ) )
		{
			Debug::Print( "L:%s\n", _fileName.c_str() );
			return path;
		}

	}


	return "";
}
