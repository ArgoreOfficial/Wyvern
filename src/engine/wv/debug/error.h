#pragma once

namespace wv {

extern const char* gErrorMessage;
extern const char* gErrorFile;
extern const char* gErrorFunction;
extern size_t gErrorLine;

void SignalHandler( int _code );
void Throw( const char* _file, const char* _function, size_t _line, const char* _message = 0 );

namespace Error {

void init();

}

}

#define WV_THROW(...) wv::Throw( __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__ )

