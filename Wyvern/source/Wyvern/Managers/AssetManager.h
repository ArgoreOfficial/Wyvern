#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Filesystem/Filesystem.h>
#include <Wyvern/Rendering/Framework/Model.h>
#include <vector>
#include <map>

namespace WV
{
	class AssetManager : ISingleton<AssetManager>
	{
	public:
		static void loadQueued() { getInstance().internalLoadQueued(); };
		static void unloadAll() { getInstance().internalUnloadAll(); }

		template<class T>
		static T* load( T** _out, const char* _path ) { return getInstance().internalLoad<T>( _out, _path ); }

		template <class T>
		static void queueLoad( std::string _path );

		// TEMPORARY
		static void loadModel( Model** _out, const char* _meshPath, const char* _texturePath ) { getInstance().internalLoadModel( _out, _meshPath, _texturePath ); }
		static Model* assembleModel( std::string _meshName, std::string _textureName ) { return getInstance().internalAssembleModel( _meshName, _textureName ); }

	private:
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

		template<class T>
		T* internalLoad( T** _out, const char* _path );
		Model* internalAssembleModel( std::string _meshName, std::string _textureName );


		void internalLoadModel( Model** _out, const char* _meshPath, const char* _texturePath );
		void internalUnloadAll();

		void internalLoadQueued();

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



		int loadingFiles = 0;
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


	template<> inline void AssetManager::queueLoad<Mesh>(std::string _path) { getInstance().m_loadQueue.push_back( { Asset::AssetType::MESH, _path } ); }
	template<> inline void AssetManager::queueLoad<Texture>(std::string _path) { getInstance().m_loadQueue.push_back( { Asset::AssetType::TEXTURE, _path } ); }
	template<> inline void AssetManager::queueLoad<ShaderSource>(std::string _path) { getInstance().m_loadQueue.push_back( { Asset::AssetType::SHADER, _path } ); }

	// ------------------- load templates ------------------- //

	template<class T>
	inline T* AssetManager::internalLoad( T** _out, const char* _path )
	{
		WVERROR( "Invalid load type " )
			return nullptr;
	}

	template<>
	inline Mesh* AssetManager::internalLoad<Mesh>( Mesh** _out, const char* _path )
	{
		MeshAsset* meshAsset = new MeshAsset();
		meshAsset->load( _path );
		
		/*
		*_out = new Mesh( *meshAsset );
		m_meshes.push_back( *_out );
		*/
		return *_out;
	}

	template<>
	inline Material* AssetManager::internalLoad<Material>( Material** _out, const char* _path )
	{

	}

	template<>
	inline ShaderProgram* AssetManager::internalLoad<ShaderProgram>( ShaderProgram** _out, const char* _path )
	{
		if ( !Filesystem::fileExists( _path, true ) ) return nullptr; // check if shader file exists

		ShaderSource shaderSource( _path );
		*_out = new ShaderProgram( shaderSource );

		// m_shaderPrograms.push_back( *_out );
		return *_out;
	}

	template<>
	inline Texture* AssetManager::internalLoad<Texture>( Texture** _out, const char* _path )
	{
		if ( !Filesystem::fileExists( _path, true ) ) return nullptr; // check if texture file exists

		// *_out = new Texture( _path );

		// m_textures.push_back( *_out );
		return *_out;
	}

}
