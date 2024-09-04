#include "MemoryDevice.h"

#ifdef WV_PLATFORM_WINDOWS
#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Shader/UniformBlock.h>

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

///////////////////////////////////////////////////////////////////////////////////////

wv::cFileSystem::~cFileSystem()
{
	if ( m_loadedMemory.size() > 0 )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "Non-Empty cFileSystem destroyed. This may cause memory leaks\n" );
		while ( m_loadedMemory.size() > 0 )
			unloadMemory( m_loadedMemory.front() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Memory* wv::cFileSystem::loadMemory( const std::string& _path )
{
#ifdef WV_PLATFORM_WINDOWS
	std::ifstream in( _path, std::ios::binary );
	if ( !in.is_open() )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load '%s'\n", _path.c_str() );
		return {};
	}

	std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

	
	Memory* mem = new Memory();
	mem->data = new unsigned char[ buf.size() ];
	mem->size = static_cast<unsigned int>( buf.size() );

	memcpy( mem->data, buf.data(), buf.size() );
	
	m_mutex.lock();
	m_loadedMemory.push_back( mem );
	m_mutex.unlock();

	return mem;
#else
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cFileSystem::unloadMemory( Memory* _memory )
{
	if ( !_memory )
		return;

	m_mutex.lock();
	
	for ( int i = 0; i < (int)m_loadedMemory.size(); i++ )
	{
		if ( m_loadedMemory[ i ] != _memory )
			continue;

		m_loadedMemory.erase( m_loadedMemory.begin() + i );
		break;
	}

	m_mutex.unlock();

	delete _memory->data;
	*_memory = {};
	delete _memory;
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

bool wv::cFileSystem::fileExists( const std::wstring& _path )
{
#ifdef WV_PLATFORM_WINDOWS
	std::filesystem::path fpath( _path ); // might not be needed on windows?
	std::ifstream f( fpath );
	return f.good();
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

std::wstring wv::cFileSystem::getFullPath( const std::string& _fileName )
{
#ifdef WV_PLATFORM_WINDOWS
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wname = converter.from_bytes( _fileName ); // convert char string to wchar string

	return getFullPath( wname );
#else
	return L"";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////

std::wstring wv::cFileSystem::getFullPath( const std::wstring& _fileName )
{
	for ( int i = 0; i < m_directories.size(); i++ )
	{
		std::wstring wpath = m_directories[ i ];
		wpath.append( _fileName );

		if ( fileExists( wpath ) )
			return wpath;
	}

	return std::wstring();
}
