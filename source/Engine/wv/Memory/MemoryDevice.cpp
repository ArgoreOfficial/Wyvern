#include "MemoryDevice.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Shader/UniformBlock.h>
#include <wv/Pipeline/Pipeline.h>

#define STB_IMAGE_IMPLEMENTATION
#include <wv/Auxiliary/stb_image.h>

#include <wv/Auxiliary/json.hpp>
#include <wv/Auxiliary/fkYAML/node.hpp>

#include <fstream>
#include <vector>

#include <wv/Debug/Print.h>

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

	m_loadedMemory.push_back( mem );
	Debug::Print( Debug::WV_PRINT_DEBUG, "Loaded '%s' @ %i bytes\n", _path.c_str(), mem->size );
	return mem;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cFileSystem::unloadMemory( Memory* _memory )
{
	if ( !_memory )
		return;
	
	for ( int i = 0; i < (int)m_loadedMemory.size(); i++ )
	{
		if ( m_loadedMemory[ i ] != _memory )
			continue;

		m_loadedMemory.erase( m_loadedMemory.begin() + i );
		break;
	}

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

wv::TextureMemory* wv::cFileSystem::loadTextureData( const std::string& _path )
{
	TextureMemory* mem = new TextureMemory();

	stbi_set_flip_vertically_on_load( 0 );
	mem->data = reinterpret_cast<uint8_t*>( stbi_load( _path.c_str(), &mem->width, &mem->height, &mem->numChannels, 0) );

	if ( !mem->data )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to load texture %s\n", _path.c_str() );
		unloadMemory( mem );
		return {}; // empty memory object
	}
	
	mem->size = mem->height * mem->numChannels * mem->width * mem->numChannels;
	m_loadedMemory.push_back( mem );
	Debug::Print( Debug::WV_PRINT_DEBUG, "Loaded '%s' (%ix%i @ %ibpp) @ %i bytes\n", _path.c_str(), mem->width, mem->height, mem->numChannels * 8, mem->size);
	return mem;
}

bool wv::cFileSystem::fileExists( const std::string& _path )
{
	std::ifstream f( _path );
	return f.good();
}
