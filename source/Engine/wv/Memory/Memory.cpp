#include "Memory.h"

#include <wv/Memory/FileSystem.h>

void wv::cMemoryStream::dump( const std::string& _path )
{
	std::ofstream file( _path );
	file.write( (const char*)data(), m_size );
	file.close();
	printf( "Dumped %s (%i bytes)\n", _path.c_str(), (int)m_size );
}

void wv::cMemoryStream::set( uint8_t* _data, size_t _size )
{
	if( m_pAllocatedData )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "cMemoryStream::set used on already allocated memory block. Deallocating old memory" );
		deallocate();
	}

	m_pAllocatedData = _data;
	m_size = _size;
	m_allocatedSize = _size;
}

void wv::cMemoryStream::clear()
{
	if( !m_pAllocatedData )
		return;

	memset( m_pAllocatedData, 0, m_allocatedSize );
	m_size = 0; // no bytes are actively used
	m_pData = m_pAllocatedData;
}

void wv::cMemoryStream::allocate( size_t _size )
{
	if( m_pAllocatedData )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "cMemoryStream::allocate used on already allocated memory block. Deallocating old memory" );
		deallocate();
	}

	m_pAllocatedData = WV_NEW_ARR( uint8_t, _size );
	m_allocatedSize = _size;

	m_pData = m_pAllocatedData;
	
	clear(); // set all bytes to 0
}

void wv::cMemoryStream::deallocate()
{
	if( !m_pAllocatedData )
		return;

	WV_FREE( m_pAllocatedData );

	m_pAllocatedData = nullptr;
	m_pData          = nullptr;

	m_size          = 0;
	m_allocatedSize = 0;
}

void wv::cMemoryStream::reallocate( size_t _newSize )
{
	uint8_t* pOldData = WV_NEW_ARR( uint8_t, m_size );
	size_t   oldSize  = m_size;

	memcpy( pOldData, m_pData, m_size );
	
	deallocate();
	allocate( _newSize );
	push( *pOldData, oldSize );

	WV_FREE( pOldData );
}
