#pragma once

#include <wv/filesystem/file_system.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

class WindowsFileSystem : public IFileSystem
{
public:
	WindowsFileSystem() = default;

	void initialize() override;

	virtual void mount( const std::string& _name ) override;

	virtual uint64_t getFileSize( FileID& _file ) override;

	virtual FileID openFile ( const char* _path, const OpenMode& _mode )             override;
	virtual int    readFile ( FileID& _file, uint8_t* _buffer, const size_t& _size ) override;
	virtual void   writeFile( FileID& _file, uint8_t* _buffer, const size_t& _size ) override;
	virtual void   closeFile( FileID& _file )                                        override;

};

}