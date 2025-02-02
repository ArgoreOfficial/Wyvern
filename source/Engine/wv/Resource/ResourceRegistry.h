#pragma once

#include <wv/Debug/Print.h>

#include <wv/Memory/FileSystem.h>
#include <wv/Memory/Memory.h>
#include <wv/Resource/Resource.h>

#include <string>
#include <unordered_map>
#include <mutex>

#include <wv/JobSystem/JobSystem.h>
#include <wv/Engine/Engine.h>


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
			m_pLowLevelGraphics{ _pLowLevelGraphics }
		{
			
		}

		~cResourceRegistry();
		
		void initializeEmbeded();

		struct LoadData
		{
			iResource* resource = nullptr;
			wv::cFileSystem* pFileSystem = nullptr;
			wv::iLowLevelGraphics* pLowLevelGraphics = nullptr;
		};

		template<typename T, typename...Args, std::enable_if_t<std::is_base_of_v<wv::iResource, T>, bool> = true>
		T* load( const std::string& _path, Args... _args )
		{
			m_mutex.lock();
			iResource* res = getLoadedResource( _path );

			if ( res == nullptr )
			{
				std::string fullPath = m_pFileSystem->getFullPath( _path );
				T* tres = WV_NEW( T,  _path, fullPath, _args... );
				res = (iResource*)tres;

				handleResourceType<T>( tres );

				Job::JobFunction_t fptr = []( const Job _job, void* _pData )
					{
						LoadData* loadData = (LoadData*)_pData;

						loadData->resource->load( loadData->pFileSystem, loadData->pLowLevelGraphics );
						WV_FREE( _pData );
					};

				JobSystem* pJobSystem = cEngine::get()->m_pJobSystem;
				LoadData* loadData = WV_NEW( LoadData, res, m_pFileSystem, m_pLowLevelGraphics );
				JobSystem::JobID job = pJobSystem->createJob( fptr, loadData );
				pJobSystem->run( &job );

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

		bool isWorking() { false; }

	protected:

		template<typename T> 
		void handleResourceType( T* _res );

		void addResource( iResource* _resource );
		 
		void findAndRemoveResource( iResource* _resource );
		void removeResource( const std::string& _name );

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