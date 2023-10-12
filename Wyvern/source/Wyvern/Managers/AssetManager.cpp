#include "AssetManager.h"
#include <thread>
using namespace WV;


Model* WV::AssetManager::internalAssembleModel( std::string _meshName, std::string _textureName )
{

	Mesh* mesh = new Mesh( *m_meshAssets[ _meshName ] );
	ShaderProgram* shaderProgram = new ShaderProgram( *m_shaderAssets[ "default.shader" ] );
	Texture* texture = new Texture( m_textureAssets[ _textureName ]->getTextureData() );

	Diffuse diffuse{ glm::vec4( 1.0f,1.0f,1.0f,1.0f ), texture };
	Material* material = new Material( shaderProgram, diffuse, glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

	return new Model( mesh, material );
}

void AssetManager::internalUnloadAll()
{
	WVDEBUG( "Unloading Assets:" );
	clearMap<std::string, MeshAsset*>( m_meshAssets );
	clearMap<std::string, TextureAsset*>( m_textureAssets );
	clearMap<std::string, ShaderSource*>( m_shaderAssets );
	WVDEBUG( "Unloading GL Objects" );
	clearMap<std::string, Mesh*>( m_meshes );
	clearMap<std::string, Material*>( m_materials );
	clearMap<std::string, ShaderProgram*>( m_shaderPrograms );
	clearMap<std::string, Texture*>( m_textures );
}

void WV::AssetManager::loadQueuedAssetThread( AssetManager* _instance )
{
	AssetManager& instance = *_instance;

	while ( instance.m_loadQueue.size() > 0 )
	{
		AssetQueueObject current = instance.m_loadQueue.front();

		std::string filename = Filesystem::getFilenameFromPath( current.path );

		switch ( current.type )
		{
		case( Asset::AssetType::MESH ):
		{
			if ( checkFileAlreadyLoaded<MeshAsset*>(instance.m_meshAssets, filename) ) break;

			MeshAsset* meshAsset = new MeshAsset();
			meshAsset->load( current.path );
			instance.m_meshAssets[ filename ] = meshAsset;

			break;
		}
		case( Asset::AssetType::TEXTURE ):
		{
			if ( checkFileAlreadyLoaded<TextureAsset*>( instance.m_textureAssets, filename ) ) break;

			TextureAsset* textureAsset = new TextureAsset();
			textureAsset->load( current.path );
			instance.m_textureAssets[ filename ] = textureAsset;

			break;
		}
		case( Asset::AssetType::SHADER ):
		{
			if ( checkFileAlreadyLoaded<ShaderSource*>( instance.m_shaderAssets, filename ) ) break;

			ShaderSource* shader = new ShaderSource( current.path );
			// shader->load( current.path );
			instance.m_shaderAssets[ filename ] = shader;

			break;
		}
		}

		instance.m_loadQueue.erase( instance.m_loadQueue.begin() );
	}

	instance.m_assetManagerMutex.lock();
	WVDEBUG( "Loading complete. Killing thread..." );
	instance.m_isLoading = false;
	instance.m_assetManagerMutex.unlock();
}

std::thread* AssetManager::internalLoadQueued()
{
	m_isLoading = true;

	WVDEBUG( "Loading assets. Creating thread..." );

	AssetManager& instance = getInstance();
	instance.m_assetLoadThread = new std::thread( AssetManager::loadQueuedAssetThread, &instance );
	return m_assetLoadThread;
}

bool AssetManager::internalIsLoading()
{
	bool out = true;
	m_assetManagerMutex.lock();
	out = m_isLoading;
	m_assetManagerMutex.unlock();

	return out;
}

