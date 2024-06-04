#pragma once

#include <stdint.h>

namespace wv
{
	class Mesh;

	struct Memory
	{
		uint8_t* data = nullptr;
		unsigned int size = 0;
	};

	struct TextureMemory : public Memory
	{
		int width = 0;
		int height = 0;
		int numChannels = 0;
	};

	class MemoryDevice
	{
	public:
		Memory loadFromFile( const char* _path );
		void freeMemory( Memory* _memory );

		TextureMemory loadTextureData( const char* _path );
		void unloadTextureData( TextureMemory* _memory );

		Mesh* loadModel( const char* _path, bool _binary = false );

	private:
		int numLoadedFiles = 0;

	};
}