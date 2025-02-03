#pragma once

#ifdef _MSC_VER
#pragma warning( disable : 4267 )
#endif

#include <cstring>
#include <stdint.h>
#include <stdexcept>
#include <set>
#include <shared_mutex>
#include <vector>

namespace arx {

template<typename _Kty, typename _Ty, typename _Cty>
struct scoped_lock_ref
{
	scoped_lock_ref( _Kty _key, _Ty& _obj, _Cty& _container ) : key{_key}, obj { _obj }, m_container{ _container }
	{ };
	_Kty key;
	_Ty& obj;

	~scoped_lock_ref() { m_container.unlock( key ); }

	operator _Ty& () { return obj; }

private:
	_Cty& m_container;
};

template<typename _Kty, typename _Ty>
class unordered_array
{
public:
	unordered_array() { }

	~unordered_array() {
		clear();

		m_buffer.clear();
	}

	template<typename...Args>
	[[nodiscard]] _Kty emplace( const Args&... _args );
	[[nodiscard]] _Ty& at( const _Kty& _key );
	[[nodiscard]] _Ty& at_locked( const _Kty& _key );
	[[nodiscard]] scoped_lock_ref<_Kty, _Ty, unordered_array<_Kty, _Ty>> at_scope_locked( const _Kty& _key );
	[[nodiscard]] bool contains( const _Kty& _key );

	[[nodiscard]] _Ty& operator[]( const _Kty& _key ) { return at( _key ); }

	void erase( const _Kty& _key );
	void clear();
	
	size_t count( void ) { return m_keys.size(); }
	size_t size ( void ) { return m_buffer.size(); }

	void lock( const _Kty& _key ) { 
	#ifdef __cpp_exceptions
		if( m_lockedkeys.find( _key ) == m_lockedkeys.end() )
			throw std::runtime_error( "Attemping to lock already locked key" );
	#endif

		m_lockedkeys.insert( _key ); 
		m_sharedMutex.lock_shared(); 
	}

	void unlock( const _Kty& _key ) { 
		m_lockedkeys.erase( _key ); 
		m_sharedMutex.unlock_shared(); 
	}

private:

	std::set<_Kty> m_keys{};
	std::set<_Kty> m_lockedkeys{};
	std::vector<_Ty> m_buffer{};
	std::shared_mutex m_sharedMutex{};
};

template<typename _Kty, typename _Ty>
template<typename... Args>
inline _Kty unordered_array<_Kty, _Ty>::emplace( const Args&... _args ) {
	size_t key = 1;
	while( contains( static_cast<_Kty>( key ) ) ) // find an unused key
		key++;
		
	m_sharedMutex.lock();
	if( key > m_buffer.size() )
	{
		m_buffer.push_back( _Ty{ _args... } );
	}
	else
	{
		size_t index = key - 1; // index is key-1 because key 0 is invalid/none
		m_buffer[ index] = _Ty{ _args... };
	}
	m_sharedMutex.unlock();

	m_keys.insert( (_Kty)key );
	return (_Kty)key;
}

template<typename _Kty, typename _Ty>
inline _Ty& unordered_array<_Kty, _Ty>::at( const _Kty& _key ) {
	size_t index = (size_t)_key - 1;
	return m_buffer[ index ];
}

template<typename _Kty, typename _Ty>
inline _Ty& unordered_array<_Kty, _Ty>::at_locked( const _Kty& _key ) {
	lock( _key );
	return at( _key );
}

template<typename _Kty, typename _Ty>
inline scoped_lock_ref<_Kty, _Ty, unordered_array<_Kty, _Ty>> unordered_array<_Kty, _Ty>::at_scope_locked( const _Kty& _key ) {
	return scoped_lock_ref<_Kty, _Ty, unordered_array<_Kty, _Ty>>{ _key, at_locked( _key ), *this };
}

template<typename _Kty, typename _Ty>
inline bool unordered_array<_Kty, _Ty>::contains( const _Kty& _key ) {
	return m_keys.count( _key ) != 0;
}

template<typename _Kty, typename _Ty>
inline void unordered_array<_Kty, _Ty>::erase( const _Kty& _key ) {
	m_keys.erase( _key );

	size_t index = (size_t)_key - 1;
	m_buffer[ index ] = _Ty{};
}

template<typename _Kty, typename _Ty>
inline void unordered_array<_Kty, _Ty>::clear() {
	std::vector<_Kty> keysToErase{};
	for( auto& key : m_keys )
		keysToErase.push_back( key );
	for( auto& key : keysToErase )
		erase( key );
		
	m_keys.clear();
}

}

#ifdef _MSC_VER
#pragma warning( default : 4267 )
#endif