#pragma once

#include <stdint.h>
#include <stdexcept>
#include <set>
#include <mutex>

namespace wv
{
	template<typename T, typename ID>
	class cObjectHandleContainer
	{
	public:
		cObjectHandleContainer() {};
		cObjectHandleContainer( size_t _maxIDs ) : m_maxIDs{ _maxIDs } {};

		void setMaxIDs( size_t _maxIDs )
		{
			if( _maxIDs < m_IDs.size() )
				return;

			m_maxIDs = _maxIDs;
		}

		template<typename...Args>
		[[nodiscard]] ID allocate( Args... _args );

		void deallocate( ID _id )
		{
			std::scoped_lock lock( m_mutex );

			if( !m_IDs.contains( _id ) )
				throw std::out_of_range( "Cannot deallocate Invalid ID" );
			
			m_IDs.erase( _id );

			size_t index = _id.value - 1;

			T& obj = ( (T*)m_pObjectBuffer )[ index ];
			obj.~T();

			memset( &obj, 0, sizeof( T ) );
		}

		T& get( ID _id )
		{
			std::scoped_lock lock( m_mutex );
			if( !m_IDs.contains( _id ) )
				throw std::out_of_range( "Invalid ID" );

			size_t index = _id.value - 1;
			
			return ( (T*)m_pObjectBuffer )[ index ];
		}

		size_t numIDs() { return m_IDs.size(); }

	private:

		std::set<ID> m_IDs;
		uint8_t* m_pObjectBuffer = nullptr;
		size_t m_size = 0;
		size_t m_maxIDs = 0;

		std::mutex m_mutex{};
	};

	template<typename T, typename ID>
	template<typename... Args>
	inline ID cObjectHandleContainer<T, ID>::allocate( Args... _args )
	{
		std::scoped_lock lock( m_mutex );
		if( m_maxIDs != 0 && m_IDs.size() >= m_maxIDs )
			throw std::runtime_error( "Cannot allocate more IDs" );

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
		T* obj = new( base ) T( _args... );

		m_IDs.insert( (ID)id );

		return (ID)id;
	}
}