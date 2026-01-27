#pragma once

#include <wv/types.h>

#include <filesystem>
#include <unordered_map>
#include <mutex>

namespace wv {

template<typename Ty>
class ReadThroughCache
{
public:
	ReadThroughCache() { }

	Ref<Ty> get( const std::filesystem::path& _path ) {
		std::scoped_lock lock( m_mtx );

		auto it = m_managed.find( _path );
		if ( it != m_managed.end() && !it->second.expired() )
			return it->second.lock();
		
		Ref<Ty> ref = std::make_shared<Ty>( _path );
		m_managed.emplace( _path, ref );

		return ref;
	}

	void add( const std::filesystem::path& _path, const Ref<Ty>& _ref )
	{
		std::scoped_lock lock( m_mtx );

		auto it = m_managed.find( _path );
		if ( it != m_managed.end() )
			throw std::runtime_error( "Key already exists" );
		
		m_managed.emplace( _path, _ref );
	}
	
protected:
	std::mutex m_mtx;
	std::unordered_map<std::filesystem::path, WeakRef<Ty>> m_managed;
};

}