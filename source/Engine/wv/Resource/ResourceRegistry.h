#pragma once

#include <wv/Debug/Print.h>

#include <wv/Memory/FileSystem.h>
#include <wv/Resource/cResourceLoader.h>
#include <wv/Resource/Resource.h>

#include <string>
#include <unordered_map>
#include <mutex>

namespace wv
{
	class iResource;
	class iLowLevelGraphics;
	class cMeshResource;

	class cResourceRegistry
	{
	public:
		cResourceRegistry( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics ):
			m_pFileSystem{ _pFileSystem },
			m_pLowLevelGraphics{ _pLowLevelGraphics },
			m_resourceLoader{_pFileSystem, _pLowLevelGraphics }
		{
			
		}

		~cResourceRegistry();
		
		void initializeEmbeded();

		template<typename T, typename...Args, std::enable_if_t<std::is_base_of_v<wv::iResource, T>, bool> = true>
		T* load( const std::string& _path, Args... _args )
		{
			m_mutex.lock();
			iResource* res = getLoadedResource( _path );

			if ( res == nullptr )
			{
				std::string fullPath = m_pFileSystem->getFullPath( _path );
				res = (iResource*)new T( _path, fullPath, _args... );
				
				handleResourceType<T>( (T*)res );
				m_resourceLoader.addLoad( res );
				addResource( res );
			}

			res->incrNumUsers();
			
			m_mutex.unlock();
			return (T*)res;
		}

		void unload( iResource* _res )
		{
			_res->decrNumUsers();

			if( _res->getNumUsers() == 0 )
			{
				_res->unload( m_pFileSystem, m_pLowLevelGraphics );
				removeResource( _res->getName() );
			}
		}
		
		// should this be moved?
		void drawMeshInstances();

		iResource* getLoadedResource( const std::string& _name );

		bool isWorking() { return m_resourceLoader.isWorking(); }

	protected:

		template<typename T> 
		void handleResourceType( T* _res );

		void addResource( iResource* _resource );
		 
		void findAndRemoveResource( iResource* _resource );
		void removeResource( const std::string& _name );

		cResourceLoader m_resourceLoader;

		cFileSystem* m_pFileSystem;
		iLowLevelGraphics* m_pLowLevelGraphics;

		std::unordered_map<std::string, iResource*> m_resources;
		std::mutex m_mutex;

		std::vector<cMeshResource*> m_meshes;
	};

	template<typename T>
	inline void cResourceRegistry::handleResourceType( T* _res )
	{
		// do nothing
	}

	template<>
	inline void cResourceRegistry::handleResourceType<wv::cMeshResource>( cMeshResource* _res )
	{
		m_meshes.push_back( _res );
	}

}