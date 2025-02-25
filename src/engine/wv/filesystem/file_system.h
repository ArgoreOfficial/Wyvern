#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>

#include <wv/types.h>
#include <wv/strong_type.hpp>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct Memory
	{
		uint8_t* data = nullptr;
		unsigned int size = 0;
	};

	typedef wv::strong_type<uint64_t, struct FileID_t> FileID;

	enum OpenMode
	{
		WV_OPEN_MODE_READ,
		WV_OPEN_MODE_WRITE,
		WV_OPEN_MODE_READWRITE,
	};

///////////////////////////////////////////////////////////////////////////////////////

	class IFileSystem
	{

	public:
		 IFileSystem();
		~IFileSystem();

		void addDirectory( const std::string& _dir ) { m_directories.push_back( _dir ); }

		// why does this return a heap allocated pointer?
		// Memory.data is already heap allocated
		Memory* loadMemory( const std::string& _path );
		void unloadMemory( Memory* _memory );

		std::string loadString( const std::string& _path );
		bool fileExists( const std::string& _path );
		std::string getFullPath( const std::string& _fileName );
		
///////////////////////////////////////////////////////////////////////////////////////

		virtual FileID   openFile( const char* _path, const OpenMode& _mode ) = 0;
		virtual uint64_t getFileSize( FileID& _file ) = 0;
		virtual int      readFile( FileID& _file, uint8_t* _buffer, const size_t& _size ) = 0;
		virtual void     writeFile( FileID& _file, uint8_t* _buffer, const size_t& _size ) = 0;
		virtual void     closeFile( FileID& _file ) = 0;

	private:
		std::vector<std::string> m_directories;
	};

}