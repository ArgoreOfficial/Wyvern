#pragma once

#include <stdint.h>
#include <stdexcept>
#include <string>

#include <wv/helpers/strong_type.hpp>

///////////////////////////////////////////////////////////////////////////////////////

// psvita compatability
#ifdef WV_PLATFORM_PSVITA
typedef unsigned int size_t;
#endif

#define WV_DEFINE_ID( _c ) typedef wv::strong_type<uint16_t, struct _c##_t> _c

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

typedef void* ( *GraphicsDriverLoadProc )( const char* _name );

typedef unsigned int GLHandle; // same as GLuint

// WV_DEFINE_ID( ResourceID );
WV_DEFINE_ID( ObjectID );

class ResourceID
{
public:
	ResourceID() :
		value{ InvalidID }
	{ }

	explicit ResourceID( uint16_t _v ) :
		value{ _v }
	{ }

	ResourceID( const ResourceID& _other ) : 
		value{ _other.value } 
	{ }

	inline void invalidate() { value = InvalidID; }
	inline bool is_valid() const { return value != InvalidID; }

	bool operator==( const ResourceID& _other ) const { return value == _other.value; }
	bool operator!=( const ResourceID& _other ) const { return value != _other.value; }
	bool operator> ( const ResourceID& _other ) const { return value > _other.value; }
	bool operator< ( const ResourceID& _other ) const { return value < _other.value; }
	uint16_t operator- ( const ResourceID& _other ) const { return value - _other.value; }
	uint16_t operator+ ( const ResourceID& _other ) const { return value + _other.value; }

	operator uint16_t() const { return value; }

	static constexpr uint16_t InvalidID = 0;
	uint16_t value = InvalidID;
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

template <>
struct std::hash<wv::ResourceID>
{
	size_t operator()( const wv::ResourceID& _type ) const { 
		return _type.value; 
	}
};
