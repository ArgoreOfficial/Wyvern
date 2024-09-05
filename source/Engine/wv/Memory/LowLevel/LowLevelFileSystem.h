#pragma once

#include <wv/Types.h>

namespace wv
{
    
    enum eOpenMode
    {
        WV_OPEN_MODE_READ,
        WV_OPEN_MODE_WRITE,
        WV_OPEN_MODE_READWRITE,
    };
      
    class cLowLevelFileSystem
    {
    public:
        cLowLevelFileSystem() { };
        virtual ~cLowLevelFileSystem() { }

        virtual wv::Handle openFile ( const char* _path, const eOpenMode& _mode )             = 0;
        virtual int        readFile ( wv::Handle& _file, char* _buffer, const size_t& _size ) = 0;
        virtual void       closeFile( wv::Handle& _file )                                     = 0;
    };

}