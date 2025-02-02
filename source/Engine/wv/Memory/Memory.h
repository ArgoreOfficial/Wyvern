#pragma once

#include <wv/Debug/Print.h>

#include <string>
#include <stdint.h>
#include <fstream>
#include <stdio.h>
#include <cstring>

#include <vector>
#include <mutex>
#include <memory>

namespace wv
{
class MemoryTracker
{
public:

	struct Entry
	{
		void* ptr;
		const char* typestr;
		size_t size;
		const char* file;
		uint32_t line;
		const char* func;
	};

	template<typename _Ty, typename... _Args>
	static _Ty* track_new( const char* _typestr, const char* _file, uint32_t _line, const char* _func, _Args... _args );

	// relinquish ownership from the tracker
	template<typename _Ty>
	static void relinquish( _Ty* _ptr ) 
	{
		m_mutex.lock();
		std::vector<Entry>::iterator itr = m_entries.begin();
		while ( itr != m_entries.end() )
		{
			if ( itr->ptr == _ptr )
				break;

			itr++;
		}

		if ( itr != m_entries.end() )
			m_entries.erase( itr );
		else
			wv::Debug::Print( wv::Debug::WV_PRINT_WARN, "Pointer was not allocated with WV_NEW()\n" );
		m_mutex.unlock();
	}

	template<typename _Ty>
	static void track_free( _Ty* _ptr )
	{
		wv::MemoryTracker::relinquish<_Ty>( _ptr );
		free( (void*)_ptr );
	}

	static size_t getNumAllocations() { return m_entries.size(); }
	static void dump()
	{
		std::scoped_lock lock{ m_mutex };
		for ( size_t i = 0; i < m_entries.size(); i++ )
		{
			Entry& entry = m_entries[ i ];
			printf( "[%p] %-32s %s %i\n", entry.ptr, entry.typestr, entry.file, entry.line );
		}
	}

private:

	static inline std::vector<Entry> m_entries{};
	static inline std::mutex m_mutex{};

	// #ifdef WV_TRACK_MEMORY
#define WV_NEW( _Ty, ... ) wv::MemoryTracker::track_new<_Ty>( #_Ty, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )
#define WV_FREE( _ptr ) wv::MemoryTracker::track_free( _ptr )
#define WV_RELINQUISH( _ptr ) wv::MemoryTracker::relinquish( _ptr )

};

class cMemoryStream
{
public:

	void set( uint8_t* _data, size_t _size );
	void clear();

	void allocate( size_t _size );
	void deallocate();
	void reallocate( size_t _newSize );

	void dump( const std::string& _path );

	template<typename T> T& at( size_t _index );

	template<typename T> void push( const T& _data, const size_t& _size );
	template<typename T> void push( const T& _data ) { push( _data, sizeof( T ) ); }

	template<typename T> T& pop( const size_t& _size );
	template<typename T> T& pop() { return pop<T>( sizeof( T ) ); }

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
inline T& cMemoryStream::at( size_t _index )
{
	if ( _index < 0 || _index >= m_size )
	{
		printf( "index out of range\n" );
		return T();
	}

	return *reinterpret_cast<T*>( &m_pData[ _index ] );
}

template<typename T>
inline void cMemoryStream::push( const T& _data, const size_t& _size )
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
inline T& cMemoryStream::pop( const size_t& _size )
{
	T* ptr = reinterpret_cast<T*>( m_pData );
	m_pData += _size;
	return *ptr;
}

template<typename _Ty, typename ..._Args>
inline _Ty* MemoryTracker::track_new( const char* _typestr, const char* _file, uint32_t _line, const char* _func, _Args ..._args )
{
	_Ty* obj = new _Ty( _args... );

	Entry entry;
	entry.ptr = obj;
	entry.typestr = _typestr;
	entry.size = sizeof( _Ty );
	entry.file = _file;
	entry.line = _line;
	entry.func = _func;

	m_mutex.lock();
	m_entries.push_back( entry );
	m_mutex.unlock();

	return obj;
}

}