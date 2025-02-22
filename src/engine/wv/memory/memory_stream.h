#pragma once

#include <wv/debug/log.h>

#include <string>
#include <stdint.h>
#include <cstring>

#include <vector>
#include <mutex>
#include <memory>

namespace wv
{

class MemoryStream
{
public:

	void set( uint8_t* _data, size_t _size );
	void clear();

	void allocate( size_t _size );
	void deallocate();
	void reallocate( size_t _newSize );

	template<typename T> T& at( size_t _index );

	template<typename T> void push( const T& _data, const size_t& _size );
	template<typename T> void push( const T& _data ) { push( _data, sizeof( T ) ); }

	template<typename T> T pop();

	inline uint8_t* data( void ) { return m_pData; }

	inline size_t size( void ) { return m_size; }
	inline size_t allocatedSize( void ) { return m_allocatedSize; }

private:

	uint8_t* m_pAllocatedData = nullptr; // allocated buffer base
	uint8_t* m_pData = nullptr; // stream buffer

	size_t m_allocatedSize = 0;
	size_t m_size = 0;

};

template<typename T>
inline T& MemoryStream::at( size_t _index )
{
	if ( _index < 0 || _index >= m_size )
	{
		printf( "index out of range\n" );
		return T();
	}

	return *reinterpret_cast<T*>( &m_pData[ _index ] );
}

template<typename T>
inline void MemoryStream::push( const T& _data, const size_t& _size )
{
	if ( _size <= 0 )
		return;

	if ( m_pData + m_size + _size >= m_pAllocatedData + m_allocatedSize ) // outside allocated buffer
		reallocate( m_size + _size );

	// push new data
	memcpy( m_pAllocatedData + m_size, &_data, _size );
	m_size += _size;
}

template<typename T>
inline T MemoryStream::pop()
{
	T tmp;
	std::memcpy( &tmp, m_pData, sizeof( T ) );
	m_pData += sizeof( T );
	return tmp;
}

}