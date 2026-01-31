#pragma once

// include size_t
#include <stddef.h>

#ifdef _HAS_EXCEPTIONS
#include <stdexcept>
#endif

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

#ifdef _HAS_EXCEPTIONS
#define WV_ASSERT_MSG(_condition, _msg) if (!(_condition)) throw std::runtime_error(_msg)
#define WV_ASSERT(_condition) WV_ASSERT_MSG(_condition, #_condition)
#else
// add fake assertion?
#define WV_ASSERT_MSG(_condition, _msg) if (!(_condition)) { return {} }
#define WV_ASSERT(_condition) WV_ASSERT_MSG(_condition, #_condition)
#endif

