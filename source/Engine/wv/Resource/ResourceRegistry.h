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

		~iResourceRegistry(){ }

		iResource* getLoadedResource( const std::string& _name ) 
		{ 
			if ( m_resources.contains( _name ) )
				return m_resources[ _name ];

			return nullptr; 
		}

		void addResource( const std::string& _name, iResource* _resource ) 
		{ 
			if ( !getLoadedResource( _name ) )
				m_resources[ _name ] = _resource;
		}

	protected:

		std::string m_name;

		cFileSystem* m_pFileSystem;
		std::unordered_map<std::string, iResource*> m_resources;

	};
}