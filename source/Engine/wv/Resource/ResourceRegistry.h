#pragma once

#include <string>

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

		iResource* getLoadedResource( const std::string& _name ) { return nullptr; }
		
	protected:

		std::string m_name;

		cFileSystem* m_pFileSystem;

		// resources
	};
}