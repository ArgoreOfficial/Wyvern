#pragma once

#include <wv/debug/log.h>

#include <string>

#include <vector>
#include <mutex>
#include <unordered_set>
#include <unordered_map>

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
		size_t count;
		const char* file;
		size_t line;
		const char* func;
		bool isArray;
	};

	typedef std::unordered_map<void*, Entry> entry_container_t;

	static void addEntry( void* _ptr, size_t _size, size_t _count, const char* _typestr, const char* _file, uint32_t _line, const char* _func, bool _isArray = false );

	// relinquish ownership from the tracker
	template<typename _Ty> static void relinquish( const entry_container_t::iterator& _itr );
	template<typename _Ty> static void relinquish( _Ty* _ptr );

	template<typename _Ty, typename... _Args> static _Ty* track_new    ( const char* _typestr, const char* _file, uint32_t _line, const char* _func, _Args ..._args );
	template<typename _Ty>                    static _Ty* track_new_arr( size_t _count, const char* _typestr, const char* _file, uint32_t _line, const char* _func );

	template<typename _Ty> static void track_free    ( _Ty* _ptr );
	template<typename _Ty> static void track_free_arr( _Ty* _ptr );

	static size_t getNumAllocations() { return m_entries.size(); }
	static size_t getTotalAllocationSize() { return m_totalAllocatedMemory; }
	static void dump();

private:

	template<typename _Ty>
	static entry_container_t::iterator _getEntry( _Ty* _ptr ) {
		entry_container_t::iterator itr = m_entries.begin();
		/*
		while( itr != m_entries.end() )
		{
			if( itr->ptr == _ptr )
				break;

			itr++;
		}
		*/
		itr = m_entries.find( _ptr );
		return itr;
	}

	static inline entry_container_t m_entries{};
	static inline std::mutex m_mutex{};

	static inline std::atomic_size_t m_totalAllocatedMemory{ 0 };

};

#ifdef WV_TRACK_MEMORY
#define WV_NEW( _Ty, ... ) wv::MemoryTracker::track_new<_Ty>( #_Ty, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )
#define WV_NEW_ARR( _Ty, _count ) wv::MemoryTracker::track_new_arr<_Ty>( _count, #_Ty , __FILE__, __LINE__, __FUNCTION__ )

#define WV_FREE( _ptr ) wv::MemoryTracker::track_free( _ptr )
#define WV_FREE_ARR( _ptr ) wv::MemoryTracker::track_free_arr( _ptr )
#define WV_RELINQUISH( _ptr ) wv::MemoryTracker::relinquish( _ptr )
#else
#define WV_NEW( _Ty, ... ) new _Ty( __VA_ARGS__ )
#define WV_NEW_ARR( _Ty, _count ) new _Ty[ _count ]

#define WV_FREE( _ptr ) delete _ptr
#define WV_FREE_ARR( _ptr ) delete[] _ptr
#define WV_RELINQUISH( _ptr ) ( void )( _ptr )
#endif

template<typename _Ty>
inline void MemoryTracker::relinquish( const entry_container_t::iterator& _itr )
{
	if ( _itr != m_entries.end() )
	{
		m_totalAllocatedMemory -= _itr->second.size;
		m_entries.erase( _itr );
		}
	else
		wv::Debug::Print( wv::Debug::WV_PRINT_WARN, "Pointer was not allocated with WV_NEW()\n" );
	}

template<typename _Ty>
inline void MemoryTracker::relinquish( _Ty* _ptr )
{
	m_mutex.lock();
	entry_container_t::iterator itr = _getEntry( _ptr );
	relinquish<_Ty>( itr );
	m_mutex.unlock();
}

template<typename _Ty, typename ..._Args>
inline _Ty* MemoryTracker::track_new( const char* _typestr, const char* _file, uint32_t _line, const char* _func, _Args ..._args )
{
	_Ty* ptr = new _Ty( _args... );
	wv::MemoryTracker::addEntry( ptr, sizeof( _Ty ), 1, _typestr, _file, _line, _func, false );
	return ptr;
}

template<typename _Ty>
inline _Ty* MemoryTracker::track_new_arr( size_t _count, const char* _typestr, const char* _file, uint32_t _line, const char* _func )
{
	_Ty* ptr = new _Ty[ _count ];
	wv::MemoryTracker::addEntry( ptr, sizeof( _Ty ), _count, _typestr, _file, _line, _func, true );
	return ptr;
}

template<typename _Ty>
inline void MemoryTracker::track_free( _Ty* _ptr )
{
	m_mutex.lock();
	entry_container_t::iterator itr = _getEntry( _ptr );
	if ( itr != m_entries.end() )
	{
		if ( itr->second.isArray )
			wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "track_free used for %s[]. Allocated at %s:%zu\n", 
							  itr->second.typestr, 
							  itr->second.file, 
							  itr->second.line );

		wv::MemoryTracker::relinquish<_Ty>( itr );
	}
	m_mutex.unlock();
	
	delete _ptr;
}

template<typename _Ty>
inline void MemoryTracker::track_free_arr( _Ty* _ptr )
{
	m_mutex.lock();
	entry_container_t::iterator itr = _getEntry( _ptr );
	if ( itr != m_entries.end() )
	{
		if ( !itr->second.isArray )
			wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "track_free_arr used for %s. Allocated at %s:%zu\n", 
							  itr->second.typestr, 
							  itr->second.file, 
							  itr->second.line );

		wv::MemoryTracker::relinquish<_Ty>( itr );
	}
	m_mutex.unlock();

	delete[] _ptr;
}

}