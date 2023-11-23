#pragma once

#include <Wyvern/Core/iSingleton.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Filesystem/cFilesystem.h>
#include <Wyvern/Renderer/cMesh.h>

#include <vector>
#include <map>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cAssetManager : public iSingleton<cAssetManager>
	{
	
	public:

		static std::thread* loadQueuedAssets( void ) { return getInstance().internalLoadQueued(); };
		static void unloadAll               ( void ) { getInstance().internalUnloadAll(); }
		static void addAssetToLoadQueue     ( iAsset* _asset );

///////////////////////////////////////////////////////////////////////////////////////

		static bool isLoading( void ) { return getInstance().internalIsLoading(); }

///////////////////////////////////////////////////////////////////////////////////////

	private:

		static void loadQueuedAssetThread( cAssetManager* _instance );

		void         internalUnloadAll ( void );
		std::thread* internalLoadQueued( void );
		bool         internalIsLoading ( void );

///////////////////////////////////////////////////////////////////////////////////////

		template<class kT, class T>
		void clearMap( std::map<kT, T> _map );

///////////////////////////////////////////////////////////////////////////////////////

		std::vector<iAsset*> m_loadQueue;

		bool m_isLoading = false;
		bool m_hasLoaded = false;
		std::mutex m_assetManagerMutex;
		std::thread* m_assetLoadThread = nullptr;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class kT, class T>
	void cAssetManager::clearMap( std::map<kT, T> _map )
	{

		for ( std::map<kT, T>::iterator itr = _map.begin(); itr != _map.end(); itr++ )
		{
			delete itr->second;
		}
		_map.clear();

	}

///////////////////////////////////////////////////////////////////////////////////////

}
