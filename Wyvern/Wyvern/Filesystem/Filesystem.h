#pragma once
#include <string>
#include <vector>

namespace bgfx { struct Memory; }

namespace WV
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
	};
}