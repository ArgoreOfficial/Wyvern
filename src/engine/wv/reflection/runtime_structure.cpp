#include "runtime_structure.h"

#include <wv/debug/log.h>

void wv::RuntimeStructure::pushSpan( const std::string& _name, uint32_t _size )
{
	if ( m_memberNameMap.contains( _name ) )
	{
		WV_LOG_ERROR( "Uniform %s already exists\n", _name.c_str() );
		return;
	}

	uint32_t offset = 0;

	if ( m_members.size() > 0 )
	{
		offset += m_members.back().offset;
		offset += _size;
	}

	m_memberNameMap[ _name ] = m_members.size();
	m_members.push_back( MemberSpan{ _name, _size, offset } );

	m_bufferSize += offset + _size;
}

size_t wv::RuntimeStructure::getMemberOffset( const std::string& _name ) const
{
	if ( !m_memberNameMap.contains( _name ) )
		return SIZE_MAX;

	MemberSpan span = m_members[ m_memberNameMap.at( _name ) ];
	return span.offset;
}
