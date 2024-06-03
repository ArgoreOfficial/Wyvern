#include "MemoryDevice.h"

#define STB_IMAGE_IMPLEMENTATION
#include <wv/Auxiliary/stb_image.h>

#include <fstream>
#include <vector>

wv::Memory wv::MemoryDevice::loadFromFile( const char* _path )
{
	Memory mem;
	
	std::ifstream in( _path, std::ios::binary );
	std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

	mem.data = new char[ buf.size() ];
	mem.size = buf.size();

	memcpy( mem.data, buf.data(), buf.size() );

	numLoadedFiles++;
	return mem;
}

wv::TextureMemory wv::MemoryDevice::loadTextureData( const char* _path )
{
	TextureMemory mem;

	numLoadedFiles++;
	stbi_set_flip_vertically_on_load( 1 );
	mem.data = reinterpret_cast<char*>(stbi_load( _path, &mem.width, &mem.height, &mem.numChannels, 0 ));

	if ( !mem.data )
	{
		printf( "Failed to load texture\n" );
		unloadTextureData( &mem );
		return {}; // empty memory object
	}
	
	mem.size = mem.height * mem.numChannels * mem.width * mem.numChannels;
	return mem;
}

void wv::MemoryDevice::unloadTextureData( TextureMemory* _memory )
{
	stbi_image_free( _memory->data );
	*_memory = {};
	numLoadedFiles--;
}
