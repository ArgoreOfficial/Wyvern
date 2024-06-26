#pragma once

#include <string>
#include <unordered_map>

namespace wv
{
	class iResource;
	class cFileSystem;

	class iResourceRegistry
	{
	public:
		iResourceRegistry( const std::string& _name, cFileSystem* _fileSystem ):
			m_name{_name},
			m_pFileSystem{_fileSystem}
		 { }

		~iResourceRegistry();

		iResource* getLoadedResource( const std::string& _name );

		void addResource( iResource* _resource );

	protected:

		void findAndUnloadResource( iResource* _resource );
		void unloadResource( const std::string& _name );

		std::string m_name;

		cFileSystem* m_pFileSystem;
		std::unordered_map<std::string, iResource*> m_resources;

	};
}