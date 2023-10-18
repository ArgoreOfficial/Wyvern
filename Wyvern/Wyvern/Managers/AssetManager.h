#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Filesystem/Filesystem.h>
// #include <Wyvern/Rendering/Framework/Model.h>
#include <Wyvern/Assets/Asset.h>

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

		static void load( std::string _path, eAssetType _type = eAssetType::AUTO );
		static const sAsset* getAsset( std::string _name );

	private:
		static void loadQueuedAssetThread( cAssetManager* _instance );
		
		void internalUnloadAll();
		std::thread* internalLoadQueued();
		bool internalIsLoading();

		template<class kT, class T>
		void clearMap( std::map<kT, T> _map );

		template<class T>
		static bool checkFileAlreadyLoaded( std::map<std::string, T> _map, std::string _key );
		
		std::vector<sAsset> m_loadQueue;
		std::map<std::string, sAsset*> m_loadedAssets;

		bool m_isLoading = false;
		bool m_hasLoaded = false;
		std::mutex m_assetManagerMutex;
		std::thread* m_assetLoadThread;
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

	template<class T>
	inline static bool cAssetManager::checkFileAlreadyLoaded( std::map<std::string, T> _map, std::string _key )
	{
		bool result = ( getInstance().m_meshAssets.find(_key) != getInstance().m_meshAssets.end() );
		if ( result ) WVDEBUG( "File already loaded %s", _key.c_str() );
		return result;
	}
}
