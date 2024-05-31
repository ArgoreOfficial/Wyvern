#include "MemoryDevice.h"

#define STB_IMAGE_IMPLEMENTATION
#include <wv/Auxiliary/stb_image.h>

wv::Memory wv::MemoryDevice::loadFromFile( const char* _path )
{
	return {};
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
