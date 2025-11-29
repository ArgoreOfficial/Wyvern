#ifdef WV_PLATFORM_WINDOWS

#include <stdio.h>
#include <Windows.h>

#include <wv/debug/error.h>
#include <signal.h>

#ifdef __cpp_exceptions
#include <stdexcept>
#endif

const char* wv::gErrorMessage = 0;
const char* wv::gErrorFile = 0;
const char* wv::gErrorFunction = 0;
size_t wv::gErrorLine = 0;

void wv::SignalHandler( int _code ) {
    // platform dependent
    printf( "error: %s\n", wv::gErrorMessage );
    exit( _code );
}

void wv::Throw( const char* _file, const char* _function, size_t _line, const char* _message )
{
    wv::gErrorMessage = _message;
    wv::gErrorFile = _file;
    wv::gErrorFunction = _function;
    wv::gErrorLine = _line;

#ifdef WV_DEBUG
#ifdef __cpp_exceptions
    throw std::runtime_error( _message );
#else
    raise( SIGSEGV );
#endif
#else // WV_DEBUG
    // launch error splat
#endif
}


#endif
