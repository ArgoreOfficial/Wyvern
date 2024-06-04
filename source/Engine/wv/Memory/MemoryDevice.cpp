#include "MemoryDevice.h"

#include <wv/Application/Application.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>

#define STB_IMAGE_IMPLEMENTATION
#include <wv/Auxiliary/stb_image.h>

#include <fstream>
#include <vector>

wv::Memory wv::MemoryDevice::loadFromFile( const char* _path )
{
	Memory mem;
	
	std::ifstream in( _path, std::ios::binary );
	std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

	mem.data = new unsigned char[ buf.size() ];
	mem.size = buf.size();

	memcpy( mem.data, buf.data(), buf.size() );

	numLoadedFiles++;
	return mem;
}

void wv::MemoryDevice::freeMemory( Memory* _memory )
{
	if ( !_memory->data )
		return;

	delete _memory->data;
	*_memory = {};
	numLoadedFiles--;
}

wv::TextureMemory wv::MemoryDevice::loadTextureData( const char* _path )
{
	TextureMemory mem;

	numLoadedFiles++;
	stbi_set_flip_vertically_on_load( 1 );
	mem.data = reinterpret_cast<uint8_t*>( stbi_load( _path, &mem.width, &mem.height, &mem.numChannels, 0 ) );

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

wv::Mesh* wv::MemoryDevice::loadModel( const char* _path, bool _binary )
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

    return nullptr;
}
