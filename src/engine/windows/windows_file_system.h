#pragma once

#include <wv/filesystem/file_system.h>

namespace wv
{

	class WindowsIFileSystem : public IFileSystem
	{
	public:
		WindowsIFileSystem() = default;

		FileID   openFile   ( const char* _path, const OpenMode& _mode )             override;
		uint64_t getFileSize( FileID& _file )                                     override;
		int      readFile   ( FileID& _file, uint8_t* _buffer, const size_t& _size ) override;
		void     writeFile  ( FileID& _file, uint8_t* _buffer, const size_t& _size ) override;
		void     closeFile  ( FileID& _file )                                     override;

	};
}