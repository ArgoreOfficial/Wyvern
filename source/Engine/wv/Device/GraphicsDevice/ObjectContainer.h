#pragma once

#include <stdint.h>
#include <stdexcept>
#include <set>

namespace wv
{
	template<typename T, typename ID>
	class cObjectContainer
	{
	public:
		cObjectContainer()
		{
			m_pObjectBuffer = new uint8_t[ sizeof( T ) ];
			m_size = sizeof( T );
			memset( m_pObjectBuffer, 0, m_size );
		}

		[[nodiscard]] ID allocate();

		void deallocate( ID _id )
		{
			if( !m_IDs.contains( _id ) )
				return;

			m_IDs.erase( _id );

			size_t index = _id.value - 1;

			T& obj = ( (T*)m_pObjectBuffer )[ index ];
			obj.~T();

			memset( &obj, 0, sizeof( T ) );
		}

		T& get( ID _id )
		{
			if( !m_IDs.contains( _id ) )
				throw std::out_of_range( "Invalid ID" );

			size_t index = _id.value - 1;
			return ( (T*)m_pObjectBuffer )[ index ];
		}

	private:

		std::set<ID> m_IDs;
		uint8_t* m_pObjectBuffer = nullptr;
		size_t m_size = 0;

	};

	template<typename T, typename ID>
	inline ID cObjectContainer<T, ID>::allocate()
	{
		uint64_t id = 1;
		while( m_IDs.contains( (ID)id ) ) 
			id++;

		uint64_t index = id - 1; // indices are 0 based, handles are 1 based
		size_t endOffset = sizeof( T ) * index + sizeof( T );

		if( endOffset >= m_size )
		{
			uint8_t* newptr = (uint8_t*)realloc( m_pObjectBuffer, endOffset );

			if( newptr == nullptr )
				throw std::runtime_error( "Something has gone horribly wrong" );
			
			m_pObjectBuffer = newptr;
			m_size = endOffset;
		}

		T* base = &( (T*)m_pObjectBuffer )[ index ];
		T* obj = new( base ) T();

		m_IDs.insert( (ID)id );

		return (ID)id;
	}
}