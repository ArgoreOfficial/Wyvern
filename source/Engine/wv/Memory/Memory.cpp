#include "Memory.h"

#include <wv/Memory/FileSystem.h>

// #include "Base64.h" // https://gist.github.com/tomykaira/f0fd86b6c73063283afe550bc5d77594

void wv::cMemory::dump( const std::string& _path )
{
	std::ofstream file( _path );
	file.write( (const char*)data(), m_size );
	file.close();
	printf( "Dumped %s (%i bytes)\n", _path.c_str(), (int)m_size );
}

void wv::cMemory::set( uint8_t* _data, size_t _size )
{
	if( m_pAllocatedData )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "cMemory::set used on already allocated memory block. Deallocating memory" );
		delete m_pAllocatedData;
	}

	m_pAllocatedData = _data;
	m_offset = 0;
	m_size = _size;
	m_allocatedSize = _size;
}

void wv::cMemory::clear()
{
	if( !m_pAllocatedData )
		return;

	memset( m_pAllocatedData, 0, m_allocatedSize );
	m_offset = 0;
	m_size = m_allocatedSize;
}

void wv::cMemory::allocate( size_t _size )
{
	if( m_pAllocatedData )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "cMemory::allocate used on already allocated memory block. Deallocating memory" );
		deallocate();
	}

	m_pAllocatedData = new uint8_t[ _size ];
	clear(); // set all bytes to 0
}

void wv::cMemory::deallocate()
{
	if( !m_pAllocatedData )
		return;

	delete m_pAllocatedData;
	m_pAllocatedData = nullptr;
	m_offset = 0;
	m_size = 0;
	m_allocatedSize = 0;
}

/*
wv::cMemory wv::cMemory::decodeBase64( const std::string& _dataString )
{	
	std::string data;
	macaron::Base64::Decode( _dataString, data );

	wv::cMemory mem;
	mem.data = new char[ data.size() ];
	mem.size = data.size();
	memcpy( mem.data, data.data(), mem.size );

    return mem;
}

std::string wv::cMemory::encodeBase64()
{
	std::string dataString( data, size );
	return macaron::Base64::Encode( dataString );
}
*/
