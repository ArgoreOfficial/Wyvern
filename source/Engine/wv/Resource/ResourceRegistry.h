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
	class iGraphicsDevice;
	class cMeshResource;

	class cResourceRegistry
	{
	public:
		cResourceRegistry( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ):
			m_pFileSystem{ _pFileSystem },
			m_pGraphicsDevice{ _pGraphicsDevice },
			m_resourceLoader{_pFileSystem, _pGraphicsDevice }
		{
			
		}

		~cResourceRegistry();
		
		void initializeEmbeded();

		template<typename T, std::enable_if_t<std::is_base_of_v<wv::iResource, T>, bool> = true>
		T* load( const std::string& _path )
		{
			iResource* res = getLoadedResource( _path );

			if ( res == nullptr )
			{
				std::string fullPath = m_pFileSystem->getFullPath( _path );
				res = (iResource*)new T( _path, fullPath );
				
				handleResourceType<T>( (T*)res );
				m_resourceLoader.addLoad( res );
				addResource( res );
			}

			res->incrNumUsers();

			return (T*)res;
		}

		void unload( iResource* _res )
		{
			_res->decrNumUsers();

			if( _res->getNumUsers() == 0 )
			{
				_res->unload( m_pFileSystem, m_pGraphicsDevice );
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
		iGraphicsDevice* m_pGraphicsDevice;

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