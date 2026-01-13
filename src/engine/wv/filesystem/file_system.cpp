#include "file_system.h"

#include <wv/debug/log.h>
#include <wv/memory/memory.h>
#include <wv/platform/platform.h>

#include <vector>
#include <string>

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
		wv::Debug::Print( "romfsInit: %08lX\n", rc );
	else
	{
		wv::Debug::Print( "romfs Init Successful!\n" );
	}
#endif
}

wv::IFileSystem::~IFileSystem()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_PLATFORM_PSVITA
#define ERROR_PREFIX "Error:"
#endif

std::vector<uint8_t> wv::IFileSystem::loadEntireFile( const std::filesystem::path& _path )
{
	std::filesystem::path path = getFullPath( _path );
	if ( path == "" )
	{
		WV_LOG_ERROR( "File '%s' does not exist\n", _path.string().c_str() );
		return {};
	}

	FileID file = openFile( path.string().c_str(), wv::OpenMode::WV_OPEN_MODE_READ );
	if( !file.is_valid() )
		return {};

	uint64_t size = getFileSize( file );

	std::vector<uint8_t> mem;
	mem.resize( size );
	
	readFile( file, mem.data(), size);
	closeFile( file );

	Debug::Print( "Loaded file '%s'\n", _path.string().c_str() );

#ifdef WV_PLATFORM_PSVITA
	if( strncmp( mem->data, ERROR_PREFIX, strlen( ERROR_PREFIX ) ) == 0 )
	{
		wv::Debug::Print( "%s\n", mem.data() );
		return nullptr;
	}
#endif

	return mem;
}

///////////////////////////////////////////////////////////////////////////////////////

std::string wv::IFileSystem::loadString( const std::filesystem::path& _path )
{
	std::vector<uint8_t> mem = loadEntireFile( _path );

	if ( mem.size() == 0 )
		return "";

	std::string str( (const char*)mem.data(), mem.size() );
	return str;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::IFileSystem::fileExists( const std::filesystem::path& _path )
{
	FileID f = openFile( _path.string().c_str(), OpenMode::WV_OPEN_MODE_READ );
	if( f.is_valid() )
	{
		closeFile( f );
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////

std::filesystem::path wv::IFileSystem::getFullPath( const std::filesystem::path& _fileName )
{
	if( fileExists( _fileName ) )
		return _fileName;
	
	std::filesystem::path path = m_mounted / _fileName;
	if( fileExists( path ) )
		return path;

	return "";
}
