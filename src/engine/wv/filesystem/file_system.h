#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>

#include <wv/types.h>
#include <wv/helpers/strong_type.hpp>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

///////////////////////////////////////////////////////////////////////////////////////

struct Memory
{
	uint8_t* data = nullptr;
	unsigned int size = 0;
};

typedef wv::strong_type<uint64_t, struct FileID_t> FileID;

enum OpenMode
{
	OpenMode_Read,
	OpenMode_Write,
	OpenMode_ReadWrite,
};

///////////////////////////////////////////////////////////////////////////////////////

class IFileSystem
{

public:
	IFileSystem();
	~IFileSystem();

	std::vector<uint8_t> loadEntireFile( const std::filesystem::path& _path );

	std::string loadString( const std::filesystem::path& _path );

	bool fileExists( const std::filesystem::path& _path );
	std::filesystem::path getFullPath( const std::filesystem::path& _fileName );
	std::filesystem::path getMountedPath( const std::filesystem::path& _fileName ) const;

///////////////////////////////////////////////////////////////////////////////////////

	virtual void initialize() = 0;

	virtual void mount( const std::string& _name ) = 0;
	
	virtual uint64_t getFileSize( FileID& _file ) = 0;

	virtual FileID openFile ( const char* _path, const OpenMode& _mode ) = 0;
	virtual int    readFile ( FileID& _file, uint8_t* _buffer, const size_t& _size ) = 0;
	virtual void   writeFile( FileID& _file, uint8_t* _buffer, const size_t& _size ) = 0;
	virtual void   closeFile( FileID& _file ) = 0;

protected:
	std::string m_mounted = "";

};

}