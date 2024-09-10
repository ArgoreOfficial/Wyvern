#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>

#include "LowLevel/LowLevelFileSystem.h"

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class sMesh;

///////////////////////////////////////////////////////////////////////////////////////

	struct Memory
	{
		uint8_t* data = nullptr;
		unsigned int size = 0;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cFileSystem
	{

	public:
		 cFileSystem();
		~cFileSystem();

		void addDirectory( const std::wstring& _dir ) { m_directories.push_back( _dir ); }

		// why does this return a heap allocated pointer?
		// Memory.data is already heap allocated
		Memory* loadMemory( const std::string& _path );
		void unloadMemory( Memory* _memory );

		std::string loadString( const std::string& _path );

		bool fileExists( const std::string& _path );
		bool fileExists( const std::wstring& _path );

		std::wstring getFullPath( const std::string& _fileName );
		std::wstring getFullPath( const std::wstring& _fileName );

///////////////////////////////////////////////////////////////////////////////////////

		cLowLevelFileSystem* m_pLowLevel = nullptr;
	private:


		std::mutex m_mutex;
		std::vector<Memory*> m_loadedMemory;
		std::vector<std::wstring> m_directories;
	};

}