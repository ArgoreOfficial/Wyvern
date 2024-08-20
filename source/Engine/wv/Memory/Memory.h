#pragma once

#include <string>
#include <stdint.h>
#include <fstream>
#include <stdio.h>

#include <wv/Debug/Print.h>

namespace wv
{
	struct cMemory 
	{
	public:

		void set( uint8_t* _data, size_t _size );
		void clear();

		void allocate( size_t _size );
		void deallocate();

		void dump( const std::string& _path );

		//static cMemory decodeBase64( const std::string& _dataString );
		//std::string encodeBase64();

		template<typename T> T at( size_t _index );

		template<typename T> void push( const T& _data ) { push(_data, sizeof(T)); }
		template<typename T> void push( const T& _data, size_t _size );

		inline uint8_t* data( void ) { return m_pAllocatedData + m_offset; }

		inline size_t size         ( void ) { return m_size; }
		inline size_t allocatedSize( void ) { return m_allocatedSize; }
 
	private:
		
		uint8_t* m_pAllocatedData = nullptr;

		size_t m_offset = 0;
		size_t m_allocatedSize = 0;
		size_t m_size = 0;

	};

	template<typename T>
	inline T cMemory::at( size_t _index )
	{
		if ( _index < 0 || _index >= m_size )
		{
			printf( "index out of range\n" );
			return T();
		}

		return *reinterpret_cast<T*>( &data()[ _index ] );
	}

	template<typename T>
	inline void cMemory::push( const T& _data, size_t _size )
	{
		Debug::Print( Debug::WV_PRINT_FATAL, "cMemory::push is not fully implemented" );
		return;


		printf( "unstable function, fix this NOW" );
		if ( _size <= 0 )
			return;

		// reallocate buffer
		uint8_t* newBuffer = new uint8_t[ m_size + _size ];
		if ( m_size > 0 )
		{
			memcpy( newBuffer, data(), m_size);
			delete m_pAllocatedData;
		}
		m_pAllocatedData = newBuffer;

		// push new data
		memcpy( m_pAllocatedData + m_size, &_data, _size );
		m_size = m_size + _size;
	}
}