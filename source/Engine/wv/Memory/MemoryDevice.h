#pragma once

#include <stdint.h>
#include <string>

namespace wv
{
	class Mesh;
	class Pipeline;

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
		~MemoryDevice();

		Memory loadFromFile( const char* _path );
		void freeMemory( Memory* _memory );

		std::string loadString( const char* _path );

		TextureMemory loadTextureData( const char* _path );
		void unloadTextureData( TextureMemory* _memory );

		Mesh* loadModel( const char* _path, bool _binary = false );

		Pipeline* loadShaderPipeline( const std::string& _path );
	private:
		int m_numLoadedFiles = 0;

	};
}