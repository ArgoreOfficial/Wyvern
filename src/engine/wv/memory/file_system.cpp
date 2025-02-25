#include "file_system.h"

#include <wv/debug/log.h>
#include <wv/engine.h>
#include <wv/graphics/graphics_device.h>
#include <wv/graphics/Uniform.h>
#include <wv/memory/memory.h>

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

#if defined( WV_PLATFORM_PSVITA )
#include "lowlevel/psp2_file_system.h"
#elif defined( WV_PLATFORM_WINDOWS )
#include "lowlevel/windows_file_system.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////

wv::FileSystem::FileSystem()
{
#if defined( WV_PLATFORM_PSVITA )
	m_pLowLevel = WV_NEW( cPSVitaFileSystem );
#elif defined( WV_PLATFORM_WINDOWS )
	m_pLowLevel = WV_NEW( WindowsFileSystem );
#endif
	addDirectory( "" );
}

wv::FileSystem::~FileSystem()
{
	if ( m_loadedMemory.size() > 0 )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "Non-Empty FileSystem destroyed. This may cause memory leaks\n" );
		while ( m_loadedMemory.size() > 0 )
			unloadMemory( m_loadedMemory.front() );
	}

	if( m_pLowLevel )
	{
		WV_FREE( m_pLowLevel );
		m_pLowLevel = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_PLATFORM_PSVITA
#define ERROR_PREFIX "Error:"
#endif

wv::Memory* wv::FileSystem::loadMemory( const std::string& _path )
{
	std::scoped_lock lock( m_mutex );

	std::string path = getFullPath( _path );
	if( path == "" )
		return nullptr;

	FileID file = m_pLowLevel->openFile( path.c_str(), wv::OpenMode::WV_OPEN_MODE_READ );
	if( !file.is_valid() )
		return nullptr;

	uint64_t size = m_pLowLevel->getFileSize( file );

	Memory* mem = WV_NEW( Memory );
	mem->data = WV_NEW_ARR( unsigned char, size );
	mem->size = static_cast<unsigned int>( size );

	m_loadedMemory.push_back( mem );

	m_pLowLevel->readFile( file, mem->data, size );
	m_pLowLevel->closeFile( file );

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

void wv::FileSystem::unloadMemory( Memory* _memory )
{
	if ( !_memory )
		return;

	std::scoped_lock lock( m_mutex );

	for ( size_t i = 0; i < m_loadedMemory.size(); i++ )
	{
		if ( m_loadedMemory[ i ] != _memory )
			continue;

		m_loadedMemory.erase( m_loadedMemory.begin() + i );
		break;
	}

	WV_FREE_ARR( _memory->data );
	*_memory = {};
	WV_FREE( _memory );
}

///////////////////////////////////////////////////////////////////////////////////////

std::string wv::FileSystem::loadString( const std::string& _path )
{
	Memory* mem = loadMemory( _path );

	if ( !mem )
		return "";

	std::string str( (const char*)mem->data, mem->size );
	unloadMemory( mem );
    return str;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::FileSystem::fileExists( const std::string& _path )
{
#ifdef WV_PLATFORM_WINDOWS
	std::ifstream f( _path );
	return f.good();
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

std::string wv::FileSystem::getFullPath( const std::string& _fileName )
{
	if( fileExists( _fileName ) )
		return _fileName;
	
	for ( size_t i = 0; i < m_directories.size(); i++ )
	{
		std::string path = "";
	#ifdef WV_PLATFORM_3DS
		path.append( "data:/" );
	#elif defined( WV_PLATFORM_WINDOWS )
		path.append( "../../data/" );
	#endif
		path.append( m_directories[ i ] );
		path.append( _fileName );

		if( fileExists( path ) )
		{
			Debug::Print( Debug::WV_PRINT_DEBUG, "Checking '%s' [yes]\n", path.c_str() );
			return path;
		}

		Debug::Print( Debug::WV_PRINT_DEBUG, "Checking '%s' [no]\n", path.c_str() );
	}


	return "";
}
