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

	bool contains( const std::filesystem::path& _path ) const {
		return m_managed.contains( _path );
	}

	Ref<Ty> get( const std::filesystem::path& _path ) {

		m_mtx.lock();
		auto it = m_managed.find( _path );
		m_mtx.unlock();
		
		if ( it != m_managed.end() && !it->second.expired() )
			return it->second.lock();
		
		Ref<Ty> ref = std::make_shared<Ty>( _path );
		
		m_mtx.lock();
		m_managed.emplace( _path, ref );
		m_mtx.unlock();

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

	/**
	 * @brief Makes asset persistent. 
	 * @brief Warning! This means the asset will not unload until 
	 * @brief application is closed OR clearPersistent() is called
	 * @param _ref Type reference
	 */
	void makePersistent( const Ref<Ty>& _ref ) {
		m_persistent.push_back( _ref );
	}

	/**
	 * @brief Clears and unloads all persistent assets. 
	 * @brief Warning! This will also unload all default assets
	 */
	void clearPersistent() {
		m_persistent.clear();
	}
	
protected:
	std::mutex m_mtx;
	std::unordered_map<std::filesystem::path, WeakRef<Ty>> m_managed;
	std::vector<Ref<Ty>> m_persistent;
};

}