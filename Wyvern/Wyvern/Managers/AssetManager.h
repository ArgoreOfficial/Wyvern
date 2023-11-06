#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Filesystem/Filesystem.h>
#include <Wyvern/Rendering/Mesh.h>
#include <vector>
#include <map>
#include <mutex>

namespace WV
{
	class cAssetManager : ISingleton<cAssetManager>
	{
	public:
		static std::thread* loadQueuedAssets() { return getInstance().internalLoadQueued(); };
		static void unloadAll() { getInstance().internalUnloadAll(); }
		static bool isLoading() { return getInstance().internalIsLoading(); }

		static void addAssetToLoadQueue( IAsset* _asset );

	private:
		static void loadQueuedAssetThread( cAssetManager* _instance );

		void internalUnloadAll();
		std::thread* internalLoadQueued();
		bool internalIsLoading();
		template<class kT, class T>
		void clearMap( std::map<kT, T> _map );

		std::vector<IAsset*> m_loadQueue;

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
