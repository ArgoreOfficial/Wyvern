#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>

#include "lowlevel/low_level_file_system.h"

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct Memory
	{
		uint8_t* data = nullptr;
		unsigned int size = 0;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class FileSystem
	{

	public:
		 FileSystem();
		~FileSystem();

		void addDirectory( const std::string& _dir ) { m_directories.push_back( _dir ); }

		// why does this return a heap allocated pointer?
		// Memory.data is already heap allocated
		Memory* loadMemory( const std::string& _path );
		void unloadMemory( Memory* _memory );

		std::string loadString( const std::string& _path );

		bool fileExists( const std::string& _path );
		
		std::string getFullPath( const std::string& _fileName );
		
///////////////////////////////////////////////////////////////////////////////////////

		ILowLevelFileSystem* m_pLowLevel = nullptr;
	private:


		std::mutex m_mutex;
		std::vector<Memory*> m_loadedMemory;
		std::vector<std::string> m_directories;
	};

}