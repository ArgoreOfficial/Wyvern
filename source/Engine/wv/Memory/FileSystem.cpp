#include <wv/Memory/FileSystem.h>

#include <wv/Engine/Engine.h>
#include <wv/Graphics/Graphics.h>
#include <wv/Graphics/Uniform.h>

#ifdef WV_PLATFORM_WINDOWS
#define STB_IMAGE_IMPLEMENTATION
#include <wv/Auxiliary/stb_image.h>
#endif

#include <fstream>
#include <vector>

#include <wv/Debug/Print.h>

#include <string>

#ifdef WV_CPP20
#include <locale>
#include <codecvt>
#include <filesystem>
#endif

#if defined( WV_PLATFORM_PSVITA )
#include "LowLevel/PSVitaFileSystem.h"
#elif defined( WV_PLATFORM_WINDOWS )
#include "LowLevel/WindowsFileSystem.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////

wv::cFileSystem::cFileSystem()
{
#if defined( WV_PLATFORM_PSVITA )
	m_pLowLevel = WV_NEW( cPSVitaFileSystem );
#elif defined( WV_PLATFORM_WINDOWS )
	m_pLowLevel = WV_NEW( cWindowsFileSystem );
#endif
	addDirectory( "" );
}

wv::cFileSystem::~cFileSystem()
{
	if ( m_loadedMemory.size() > 0 )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "Non-Empty cFileSystem destroyed. This may cause memory leaks\n" );
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

wv::Memory* wv::cFileSystem::loadMemory( const std::string& _path )
{
	std::scoped_lock lock( m_mutex );

	FileID file = m_pLowLevel->openFile( _path.c_str(), wv::eOpenMode::WV_OPEN_MODE_READ );
	uint64_t size = m_pLowLevel->getFileSize( file );
	
	Memory* mem = WV_NEW( Memory );
	mem->data = WV_NEW_ARR( unsigned char, size );
	mem->size = static_cast<unsigned int>( size );

	m_loadedMemory.push_back( mem );

	m_pLowLevel->readFile( file, mem->data, size );
	m_pLowLevel->closeFile( file );

	Debug::Print( "Loaded file '%s'\n", _path.c_str() );

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

void wv::cFileSystem::unloadMemory( Memory* _memory )
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

	WV_FREE( _memory->data );
	*_memory = {};
	WV_FREE( _memory );
}

///////////////////////////////////////////////////////////////////////////////////////

std::string wv::cFileSystem::loadString( const std::string& _path )
{
	Memory* mem = loadMemory( _path );

	if ( !mem )
		return "";

	std::string str( (const char*)mem->data, mem->size );
	unloadMemory( mem );
    return str;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::cFileSystem::fileExists( const std::string& _path )
{
#ifdef WV_PLATFORM_WINDOWS
	std::ifstream f( _path );
	return f.good();
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

std::string wv::cFileSystem::getFullPath( const std::string& _fileName )
{
	for ( size_t i = 0; i < m_directories.size(); i++ )
	{
		std::string path = m_directories[ i ];
		path.append( _fileName );

		if ( fileExists( path ) )
			return path;
	}

	return "";
}
