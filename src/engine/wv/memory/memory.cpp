#include "memory.h"

void wv::MemoryTracker::addEntry( void* _ptr, size_t _size, size_t _count, const char* _typestr, const char* _file, uint32_t _line, const char* _func, bool _isArray )
{
	Entry entry;
	entry.ptr = _ptr;
	entry.typestr = _typestr;
	entry.size = _size;
	entry.count = _count;
	entry.file = _file;
	entry.line = _line;
	entry.func = _func;
	entry.isArray = _isArray;

	m_totalAllocatedMemory += _size;

	m_mutex.lock();
	m_entries.insert( { _ptr, entry } );
	m_mutex.unlock();
}

void wv::MemoryTracker::dump()
{
	std::scoped_lock lock{ m_mutex };

	if ( m_entries.empty() )
	{
		wv::Debug::Print( "No allocated memory\n" );
		return;
	}

	wv::Debug::Print( "------- Allocations : %zu ------- \n", m_entries.size() );
	for ( auto& entry : m_entries )
	{
		std::string typestr = entry.second.typestr;
		if ( entry.second.count > 1 )
			typestr += "[" + std::to_string( entry.second.count ) + "]";

		wv::Debug::Print( "[%p] %-32s %s %zu\n", entry.second.ptr, typestr.c_str(), entry.second.file, entry.second.line );
	}
}
