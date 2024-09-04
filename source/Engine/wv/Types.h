#pragma once

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////

// psvita compatability
#ifdef WV_PLATFORM_PSP2
typedef unsigned int size_t;
#endif

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	typedef void* ( *GraphicsDriverLoadProc )( const char* _name );
	typedef uint32_t Handle;
	typedef uint64_t UUID;

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
		WV_GRAPHICS_API_OPENGL,
		WV_GRAPHICS_API_OPENGL_ES1,
		WV_GRAPHICS_API_OPENGL_ES2
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

	template<typename T>
	struct sUniqueHandle
	{
	public:
		sUniqueHandle( wv::Handle _value ) : m_value( _value ) { }

		const wv::Handle& value  ( void ) const { return m_value; }
		const bool        isValid( void ) const { return m_value > 0; }

		void invalidate( void ) { m_value = 0; }

		friend bool operator==( const sUniqueHandle<T>& _l, const sUniqueHandle<T>& _r ) { return _l.m_value == _r.m_value; }
		friend bool operator <( const sUniqueHandle<T>& _l, const sUniqueHandle<T>& _r ) { return _l.m_value <  _r.m_value; }

	private:
		wv::Handle m_value;

	};

	#define DEFINE_UNIQUE_HANDLE( _name ) struct handletag__##_name {}; typedef sUniqueHandle<handletag__##_name> h##_name

}