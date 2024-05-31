#pragma once

namespace wv
{
	struct Memory
	{
		char* data = nullptr;
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
		TextureMemory loadTextureData( const char* _path );
		void unloadTextureData( TextureMemory* _memory );

	private:

		int numLoadedFiles = 0;

	};
}