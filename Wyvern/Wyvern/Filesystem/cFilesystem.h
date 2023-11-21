#pragma once
#include <string>
#include <vector>

namespace bgfx { struct Memory; }

namespace wv
{
	struct sAsset;

	class Filesystem
	{
	public:
		static bool fileExists( std::string _path, bool _printError = false );
		static std::string getFilenameFromPath( std::string _path, bool _keepExtension = true );
		static std::string getFileExtension( std::string _path );
		static const bgfx::Memory* loadMemoryFromFile( std::string _path );
		static std::vector<char>* loadByteArrayFromPath( std::string _path );

		template<class T>
		static const bgfx::Memory* vectorToMemory( std::vector<T>* _vec );
	};

	template<class T>
	inline const bgfx::Memory* Filesystem::vectorToMemory( std::vector<T>* _vec )
	{
		uint32_t size = _vec->size() * sizeof(T);

		const bgfx::Memory* mem = bgfx::alloc( size + 1 );
		memcpy( mem->data, _vec->data(), size );

		mem->data[ mem->size - 1 ] = '\0';
		return mem;
	}
}