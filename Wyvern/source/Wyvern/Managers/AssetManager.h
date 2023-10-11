#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Filesystem/Filesystem.h>
#include <Wyvern/Rendering/Framework/Model.h>
#include <vector>
#include <map>
#include <mutex>

namespace WV
{
	class AssetManager : ISingleton<AssetManager>
	{
	public:
		static std::thread* loadQueuedAssets() { return getInstance().internalLoadQueued(); };
		static void unloadAll() { getInstance().internalUnloadAll(); }
		static bool isLoading() { return getInstance().internalIsLoading(); }
		template <class T>
		static void load( std::string _path );

		// TEMPORARY
		static Model* assembleModel( std::string _meshName, std::string _textureName ) { return getInstance().internalAssembleModel( _meshName, _textureName ); }

	private:
		static void loadQueuedAssetThread( AssetManager* _instance );
		struct AssetQueueObject
		{
			Asset::AssetType type;
			std::string path;
		};
		struct InitQueueObject
		{
			Asset::AssetType type;
			std::string name;
		};

		Model* internalAssembleModel( std::string _meshName, std::string _textureName );

		void internalUnloadAll();
		std::thread* internalLoadQueued();
		bool internalIsLoading();

		template<class kT, class T>
		void clearMap( std::map<kT, T> _map );

		std::vector<AssetQueueObject> m_loadQueue;
		std::map<std::string, MeshAsset*> m_meshAssets;
		std::map<std::string, TextureAsset*> m_textureAssets;
		std::map<std::string, ShaderSource*> m_shaderAssets;
		
		// TEMP, MOVE TO DIFFERENT MANAGER
		std::vector<Model> m_renderObjects;
		std::map<std::string, Mesh*> m_meshes;
		std::map<std::string, Material*> m_materials;
		std::map<std::string, ShaderProgram*> m_shaderPrograms;
		std::map<std::string, Texture*> m_textures;

		bool m_isLoading = false;
		std::mutex m_assetManagerMutex;
		std::thread* m_assetLoadThread;
	};

	template<class kT, class T>
	inline void AssetManager::clearMap( std::map<kT, T> _map )
	{
		for ( std::map<kT, T>::iterator itr = _map.begin(); itr != _map.end(); itr++ )
		{
			delete itr->second;
		}
		_map.clear();
	}

	// ------------------- load templates ------------------- //
	
	template<class T> void AssetManager::load( std::string _path ) { WVERROR( "Invalid load type" ); }
	template<> inline void AssetManager::load<Mesh>(std::string _path) { getInstance().m_loadQueue.push_back( { Asset::AssetType::MESH, _path } ); }
	template<> inline void AssetManager::load<Texture>(std::string _path) { getInstance().m_loadQueue.push_back( { Asset::AssetType::TEXTURE, _path } ); }
	template<> inline void AssetManager::load<ShaderSource>(std::string _path) { getInstance().m_loadQueue.push_back( { Asset::AssetType::SHADER, _path } ); }


}
