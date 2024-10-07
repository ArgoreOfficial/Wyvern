#pragma once

#include "LowLevelFileSystem.h"

namespace wv
{

	class cWindowsFileSystem : public iLowLevelFileSystem
	{
	public:
		cWindowsFileSystem() = default;

		FileID   openFile   ( const char* _path, const eOpenMode& _mode )             override;
		uint64_t getFileSize( FileID& _file )                                     override;
		int      readFile   ( FileID& _file, uint8_t* _buffer, const size_t& _size ) override;
		void     writeFile  ( FileID& _file, uint8_t* _buffer, const size_t& _size ) override;
		void     closeFile  ( FileID& _file )                                     override;

	};
}