#pragma once

#include <wv/types.h>

#include <filesystem>
#include <unordered_map>

namespace wv {

template<typename Ty>
class ReadThroughCache
{
public:
	ReadThroughCache() { }

	Ref<Ty> get( const std::filesystem::path& _path ) {
		auto it = m_managed.find( _path );
		if ( it != m_managed.end() )
			return it->second.lock();

		Ref<Ty> ref = std::make_shared<Ty>( _path );
		m_managed.emplace( _path, ref );
		return ref;
	}
	
protected:
	std::unordered_map<std::filesystem::path, WeakRef<Ty>> m_managed;
};

}