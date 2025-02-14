#include "MemoryStream.h"

#include <wv/Memory/FileSystem.h>
#include <wv/Memory/Memory.h>

#include <fstream>
#include <stdio.h>

void wv::MemoryStream::set( uint8_t* _data, size_t _size )
{
	if ( m_pAllocatedData )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "cMemoryStream::set used on already allocated memory block. Deallocating old memory" );
		deallocate();
	}

	m_pAllocatedData = _data;
	m_size = _size;
	m_allocatedSize = _size;
}

void wv::MemoryStream::clear()
{
	if ( !m_pAllocatedData )
		return;

	memset( m_pAllocatedData, 0, m_allocatedSize );
	m_size = 0; // no bytes are actively used
	m_pData = m_pAllocatedData;
}

void wv::MemoryStream::allocate( size_t _size )
{
	if ( m_pAllocatedData )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "cMemoryStream::allocate used on already allocated memory block. Deallocating old memory" );
		deallocate();
	}

	m_pAllocatedData = WV_NEW_ARR( uint8_t, _size );
	m_allocatedSize = _size;

	m_pData = m_pAllocatedData;

	clear(); // set all bytes to 0
}

void wv::MemoryStream::deallocate()
{
	if ( !m_pAllocatedData )
		return;

	WV_FREE_ARR( m_pAllocatedData );

	m_pAllocatedData = nullptr;
	m_pData = nullptr;

	m_size = 0;
	m_allocatedSize = 0;
}

void wv::MemoryStream::reallocate( size_t _newSize )
{
	uint8_t* pOldData = WV_NEW_ARR( uint8_t, m_size );
	size_t   oldSize = m_size;

	memcpy( pOldData, m_pData, m_size );

	deallocate();
	allocate( _newSize );
	push( *pOldData, oldSize );

	WV_FREE_ARR( pOldData );
}
