#include "MemoryDevice.h"

#include <wv/Application/Application.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Shader/UniformBlock.h>
#include <wv/Pipeline/Pipeline.h>

#define STB_IMAGE_IMPLEMENTATION
#include <wv/Auxiliary/stb_image.h>

#include <wv/Auxiliary/json.hpp>
#include <wv/Auxiliary/fkYAML/node.hpp>

#include <fstream>
#include <vector>

wv::MemoryDevice::~MemoryDevice()
{
	if ( m_loadedMemory.size() > 0 )
	{
		printf( "Non-Empty MemoryDevice destroyed. This may cause memory leaks\n" );
		while ( m_loadedMemory.size() > 0 )
			unloadMemory( m_loadedMemory.front() );
	}
}

wv::Memory* wv::MemoryDevice::loadMemory( const char* _path )
{
	std::ifstream in( _path, std::ios::binary );
	if ( !in.is_open() )
	{
		printf( "Couldn't open file '%s'\n", _path );
		return {};
	}

	std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

	Memory* mem = new Memory();
	mem->data = new unsigned char[ buf.size() ];
	mem->size = static_cast<unsigned int>( buf.size() );

	memcpy( mem->data, buf.data(), buf.size() );

	m_loadedMemory.push_back( mem );
	return mem;
}

void wv::MemoryDevice::unloadMemory( Memory* _memory )
{
	if ( !_memory->data )
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

std::string wv::MemoryDevice::loadString( const char* _path )
{
	Memory* mem = loadMemory( _path );
	std::string str( (const char*)mem->data, mem->size );
	unloadMemory( mem );
    return str;
}

wv::TextureMemory* wv::MemoryDevice::loadTextureData( const char* _path )
{
	TextureMemory* mem = new TextureMemory();

	stbi_set_flip_vertically_on_load( 0 );
	mem->data = reinterpret_cast<uint8_t*>( stbi_load( _path, &mem->width, &mem->height, &mem->numChannels, 0 ) );

	if ( !mem->data )
	{
		printf( "Failed to load texture %s\n", _path );
		unloadMemory( mem );
		return {}; // empty memory object
	}
	
	printf( "Loaded texture '%s' (%ix%i @ %ibpp)\n", _path, mem->width, mem->height, mem->numChannels * 8 );
	mem->size = mem->height * mem->numChannels * mem->width * mem->numChannels;
	m_loadedMemory.push_back( mem );
	return mem;
}
