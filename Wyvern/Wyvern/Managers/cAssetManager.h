#pragma once
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Core/iSingleton.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Filesystem/cFilesystem.h>
#include <Wyvern/Renderer/cMesh.h>
#include <vector>
#include <map>
#include <mutex>

namespace wv
{
	class cAssetManager : iSingleton<cAssetManager>
	{
	public:
		static std::thread* loadQueuedAssets() { return getInstance().internalLoadQueued(); };
		static void unloadAll() { getInstance().internalUnloadAll(); }
		static bool isLoading() { return getInstance().internalIsLoading(); }

		static void addAssetToLoadQueue( iAsset* _asset );

	private:
		static void loadQueuedAssetThread( cAssetManager* _instance );

		void internalUnloadAll();
		std::thread* internalLoadQueued();
		bool internalIsLoading();
		template<class kT, class T>
		void clearMap( std::map<kT, T> _map );

		std::vector<iAsset*> m_loadQueue;

		bool m_isLoading = false;
		bool m_hasLoaded = false;
		std::mutex m_assetManagerMutex;
		std::thread* m_assetLoadThread = nullptr;
	};

	template<class kT, class T>
	inline void cAssetManager::clearMap( std::map<kT, T> _map )
	{
		for ( std::map<kT, T>::iterator itr = _map.begin(); itr != _map.end(); itr++ )
		{
			delete itr->second;
		}
		_map.clear();
	}
}
