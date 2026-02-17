#pragma once

#include <wv/helpers/strong_type.hpp>
#include <wv/resource_id.h>
#include <wv/string.h>

#include <stdint.h>
#include <stdexcept>
#include <string>
#include <memory>

///////////////////////////////////////////////////////////////////////////////////////

// psvita compatability
#ifdef WV_PLATFORM_PSVITA
typedef unsigned int size_t;
#endif

#define WV_DEFINE_ID( _c ) typedef wv::strong_type<uint16_t, struct _c##_t> _c

namespace wv {

///////////////////////////////////////////////////////////////////////////////////////

typedef void* ( *GraphicsDriverLoadProc )( const char* _name );

typedef unsigned int GLHandle; // same as GLuint

// WV_DEFINE_ID( ResourceID );
WV_DEFINE_ID( ObjectID );

template<typename Ty>
using Ref = std::shared_ptr<Ty>;

template<typename Ty>
using WeakRef = std::weak_ptr<Ty>;

///////////////////////////////////////////////////////////////////////////////////////

class UUID
{
public:
	UUID() : m_value{ 0 } { };
	UUID( uint64_t _value ) : m_value{ _value } { };

	std::string toString() const {
		return std::format( "0x{:x}", m_value );
	}

	bool     operator==( const wv::UUID& _other ) const { return m_value == _other.m_value; }
	bool     operator!=( const wv::UUID& _other ) const { return m_value != _other.m_value; }
	bool     operator> ( const wv::UUID& _other ) const { return m_value > _other.m_value; }
	bool     operator< ( const wv::UUID& _other ) const { return m_value < _other.m_value; }
	uint64_t operator- ( const wv::UUID& _other ) const { return m_value - _other.m_value; }
	uint64_t operator+ ( const wv::UUID& _other ) const { return m_value + _other.m_value; }

	operator uint64_t() const { return m_value; }

private:
	uint64_t m_value;
};

///////////////////////////////////////////////////////////////////////////////////////

enum DataType
{
	WV_BYTE,
	WV_UNSIGNED_BYTE,
	WV_SHORT,
	WV_UNSIGNED_SHORT,
	WV_INT,
	WV_UNSIGNED_INT,
	WV_FLOAT,
#ifndef EMSCRIPTEN
	WV_DOUBLE
#endif
};

///////////////////////////////////////////////////////////////////////////////////////

enum GraphicsAPI
{
	WV_GRAPHICS_API_NONE

#ifdef WV_SUPPORT_OPENGL
	, WV_GRAPHICS_API_OPENGL
	, WV_GRAPHICS_API_OPENGL_ES1
	, WV_GRAPHICS_API_OPENGL_ES2
#endif

#ifdef WV_PLATFORM_PSVITA
	, WV_GRAPHICS_API_PSVITA
#endif

#ifdef WV_PLATFORM_3DS
	, WV_GRAPHICS_API_CITRA
#endif
};

///////////////////////////////////////////////////////////////////////////////////////

union GenericVersion
{
	struct
	{
		unsigned short major;
		unsigned short minor;
	};
	int i;

	inline bool isAtleast( unsigned short _major, unsigned short _minor ) { return ( major >= _major ) && ( major > _major || minor >= _minor ); }

};

///////////////////////////////////////////////////////////////////////////////////////

}

namespace std {

template <>
struct hash<wv::UUID>
{
	size_t operator()( const wv::UUID& _type ) const { return hash<uint64_t>()( _type ); }
};

}
