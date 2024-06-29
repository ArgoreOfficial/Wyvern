#pragma once

#include <wv/Resource/cResourceLoader.h>

#include <string>
#include <unordered_map>

namespace wv
{
	class iResource;
	class cFileSystem;

	class iResourceRegistry
	{
	public:
		iResourceRegistry( const std::string& _name, cFileSystem* _pFileSystem ):
			m_name{_name},
			m_pFileSystem{ _pFileSystem },
			m_resourceLoader{_pFileSystem}
		 { }

		~iResourceRegistry();

		iResource* getLoadedResource( const std::string& _name );

		void addResource( iResource* _resource );

		void update();
		bool isLoading() { return m_resourceLoader.isLoading(); }

	protected:

		void findAndUnloadResource( iResource* _resource );
		void unloadResource( const std::string& _name );

		cResourceLoader m_resourceLoader;

		std::string m_name;

		cFileSystem* m_pFileSystem;
		std::unordered_map<std::string, iResource*> m_resources;

	};
}