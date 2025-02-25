#pragma once

#include <wv/debug/log.h>

#include <wv/memory/file_system.h>
#include <wv/memory/memory.h>
#include <wv/resource/resource.h>

#include <string>
#include <unordered_map>
#include <mutex>

#include <wv/job/job_system.h>
#include <wv/engine.h>


namespace wv
{
	class IResource;
	class IGraphicsDevice;
	class MeshResource;

	class ResourceRegistry
	{
	public:
		ResourceRegistry( FileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice, JobSystem* _pJobSystem ):
			m_pFileSystem{ _pFileSystem },
			m_pGraphicsDevice{ _pGraphicsDevice },
			m_pJobSystem{ _pJobSystem }
		{
			m_resourceFence = _pJobSystem->createFence();
		}

		~ResourceRegistry();

		struct LoadData
		{
			LoadData( IResource* _resource, wv::FileSystem* _pFileSystem, wv::IGraphicsDevice* _pGraphicsDevice ) : 
				resource{ _resource },
				pFileSystem{ _pFileSystem },
				pGraphicsDevice{ _pGraphicsDevice }
			{}
			IResource* resource = nullptr;
			wv::FileSystem* pFileSystem = nullptr;
			wv::IGraphicsDevice* pGraphicsDevice = nullptr;
		};

		template<typename T, typename...Args, std::enable_if_t<std::is_base_of_v<wv::IResource, T>, bool> = true>
		T* load( const std::string& _path, Args... _args )
		{
			m_mutex.lock();
			IResource* res = getLoadedResource( _path );

			if ( res == nullptr )
			{
				std::string fullPath = m_pFileSystem->getFullPath( _path );
				T* tres = WV_NEW( T,  _path, fullPath, _args... );
				res = (IResource*)tres;

				handleResourceType<T>( tres );

				Job::JobFunction_t fptr = []( void* _pData )
					{
						LoadData* loadData = (LoadData*)_pData;

						loadData->resource->load( loadData->pFileSystem, loadData->pGraphicsDevice );
						WV_FREE( loadData );
					};

				JobSystem* pJobSystem = Engine::get()->m_pJobSystem;
				LoadData* loadData = WV_NEW( LoadData, res, m_pFileSystem, m_pGraphicsDevice );
				Job* job = pJobSystem->createJob( m_resourceFence, nullptr, fptr, loadData );
				pJobSystem->submit( { job } );

				addResource( res );
			}

			res->incrNumUsers();
			
			m_mutex.unlock();
			return (T*)res;
		}

		void unload( IResource* _res )
		{
			if ( !_res )
				return;

			_res->decrNumUsers();

			if( _res->getNumUsers() == 0 )
			{
				_res->unload( m_pFileSystem, m_pGraphicsDevice );
				removeResource( _res->getName() );
			}
		}
		
		// should this be moved?
		void drawMeshInstances();

		IResource* getLoadedResource( const std::string& _name );

		bool isWorking() { 
			uint32_t v = m_resourceFence->counter.load();
			return v > 0;
		}

		void waitForFence() {
			m_pJobSystem->waitForFence( m_resourceFence );
		}

		size_t getNumLoadedResources() {
			return m_resources.size();
		}

		wv::Fence* getResourceFence() {
			return m_resourceFence;
		}
		
	protected:

		template<typename T> 
		void handleResourceType( T* _res );

		void addResource( IResource* _resource );
		 
		void findAndRemoveResource( IResource* _resource );
		void removeResource( const std::string& _name );

		FileSystem* m_pFileSystem{ nullptr };
		IGraphicsDevice* m_pGraphicsDevice{ nullptr };
		JobSystem* m_pJobSystem{ nullptr };

		std::unordered_map<std::string, IResource*> m_resources{};
		std::mutex m_mutex{};
		wv::Fence* m_resourceFence{ nullptr };

		std::vector<MeshResource*> m_meshes;
	};

	template<typename T>
	inline void ResourceRegistry::handleResourceType( T* _res )
	{
		// do nothing
	}

	template<>
	inline void ResourceRegistry::handleResourceType<wv::MeshResource>( MeshResource* _res )
	{
		m_meshes.push_back( _res );
	}

}