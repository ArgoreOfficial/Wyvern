#pragma once

#include <wv/Resource/cResourceLoader.h>

#include <wv/Debug/Print.h>

#include <string>
#include <unordered_map>
#include <mutex>

namespace wv
{
	class iResource;
	class cFileSystem;
	class iGraphicsDevice;

	class iResourceRegistry
	{
	public:
		iResourceRegistry( const std::string& _name, cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ):
			m_name{_name},
			m_pFileSystem{ _pFileSystem },
			m_resourceLoader{_pFileSystem, _pGraphicsDevice }
		{
			wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Initializing %s\n", _name.c_str() );
		}

		~iResourceRegistry();

		iResource* getLoadedResource( const std::string& _name );

		void addResource( iResource* _resource );

		void update();
		bool isWorking() { return m_resourceLoader.isWorking(); }

	protected:

		void findAndUnloadResource( iResource* _resource );
		void unloadResource( const std::string& _name );

		cResourceLoader m_resourceLoader;

		std::string m_name;

		cFileSystem* m_pFileSystem;
		std::unordered_map<std::string, iResource*> m_resources;
		std::mutex m_mutex;
	};
}