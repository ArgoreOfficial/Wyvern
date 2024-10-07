#pragma once

#include <wv/Types.h>


namespace wv
{    
    typedef wv::sStrongID<uint64_t, struct FileID_t> FileID;
    
    enum eOpenMode
    {
        WV_OPEN_MODE_READ,
        WV_OPEN_MODE_WRITE,
        WV_OPEN_MODE_READWRITE,
    };
      
    class iLowLevelFileSystem
    {
    public:
        iLowLevelFileSystem() { };
        virtual ~iLowLevelFileSystem() { }

        virtual FileID   openFile   ( const char* _path, const eOpenMode& _mode )             = 0;
        virtual uint64_t getFileSize( FileID& _file )                                     = 0;
        virtual int      readFile   ( FileID& _file, uint8_t* _buffer, const size_t& _size ) = 0;
        virtual void     writeFile  ( FileID& _file, uint8_t* _buffer, const size_t& _size ) = 0;
        virtual void     closeFile  ( FileID& _file )                                     = 0;
    };

}