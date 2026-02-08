#pragma once

#include <queue>
#include <vector>

#include <wv/memory/memory.h>

namespace wv {

template<typename Ty>
class Pool
{
public:
	Ty* allocate() {
		Ty* ptr = nullptr;

		std::scoped_lock lock{ m_mtx };

		if ( m_available.empty() )
		{
			ptr = WV_NEW( Ty );
			m_pool.push_back( ptr );
		}
		else
		{
			ptr = m_available.front();
			m_available.pop();
		}

		return ptr;
	}

	void free( Ty* _ptr ) {
		std::scoped_lock lock{ m_mtx };

		m_available.push( _ptr );
	}

	void clear() {
		std::scoped_lock lock{ m_mtx };

		for ( Ty* ptr : m_pool )
			WV_FREE( ptr );
		
		m_pool.clear();
		while ( !m_available.empty() )
			m_available.pop();
	}

	void reset() {
		std::scoped_lock lock{ m_mtx };

		while ( !m_available.empty() )
			m_available.pop();

		for ( Ty* ptr : m_pool )
			m_available.push( ptr );
	}

private:
	std::mutex m_mtx;
	
	std::vector<Ty*> m_pool      = {};
	std::queue<Ty*>  m_available = {};
};

template<typename Ty>
class GenericRingPool
{
public:
	void initialize( uint32_t _cycleSize )
	{
		m_cycleSize = _cycleSize;
		m_objects.resize( _cycleSize );
	}

	void shutdown() {
		m_objects.clear();
	}

	void setCycle( uint32_t _cycle ) {
		m_cycleIndex = _cycle % m_cycleSize;
	}

	Ty  get() const { return m_objects[ m_cycleIndex ]; }
	Ty& get()       { return m_objects[ m_cycleIndex ]; }
	
	std::vector<Ty>& getObjects() { return m_objects; }
private:
	uint32_t m_cycleSize{ 0 };
	uint32_t m_cycleIndex{ 0 };

	std::vector<Ty> m_objects{};
};

}