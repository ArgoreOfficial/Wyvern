#include "AssetManager.h"

#include <chrono>

using namespace WV;

bool AssetManager::isLoading()
{
	bool result = false;

	getInstance().m_loadThreadMutex.lock();
	result = getInstance().m_isLoading;
	getInstance().m_loadThreadMutex.unlock();

	return result;
}

void WV::AssetManager::startLoadingThread()
{
	AssetManager* instance = &getInstance();
	
	getInstance().m_isLoading = true;
	getInstance().m_loadThread = std::thread(AssetManager::loadQueuedAssets, instance);
}

void WV::AssetManager::glInitQueued()
{
	AssetManager& instance = getInstance();
	
	while ( instance.m_glInitQueue.size() > 0 )
	{
		glInitQueueObject current = instance.m_glInitQueue.front();

		switch ( current.assetType )
		{
		case( AssetType::MODEL ):

			break;
		case( AssetType::MESH ):

			( *( (Mesh**)current.out ) )->vertexArray->glInit();
			( *( (Mesh**)current.out ) )->indexBuffer->glInit();

			break;
		case( AssetType::MATERIAL ):

			break;
		case( AssetType::SHADER_PROGRAM ):

			( *( (ShaderProgram**)current.out ) )->glInit();

			break;
		case( AssetType::TEXTURE ):

			( *( (Texture**)current.out ) )->glInit();

			break;
		}

		instance.m_glInitQueue.erase( instance.m_glInitQueue.begin() );
	}
}

void WV::AssetManager::internalLoadModel( Model** _out, const char* _meshPath, const char* _texturePath )
{
	WVDEBUG( "Creating Model...", _meshPath );

	Mesh* mesh;
	AssetManager::load<Mesh>( &mesh, _meshPath );

	ShaderProgram* shaderProgram;
	AssetManager::load<ShaderProgram>( &shaderProgram, "shaders/default.shader" );

	Texture* texture;
	AssetManager::load<Texture>( &texture, _texturePath );

	Diffuse diffuse{ glm::vec4( 1.0f,1.0f,1.0f,1.0f ), texture };
	Material* material = new Material( shaderProgram, diffuse, glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

	*_out = new Model( mesh, material );
}

void AssetManager::internalUnloadAll()
{
	WVDEBUG( "Destroying Meshes..." );
	while ( m_meshes.size() > 0 )
	{
		delete m_meshes.back();
		m_meshes.back() = nullptr;
		m_meshes.pop_back();
	}

	WVDEBUG( "Destroying materials..." );
	while ( m_materials.size() > 0 )
	{
		delete m_materials.back();
		m_materials.back() = nullptr;
		m_materials.pop_back();
	}

	WVDEBUG( "Destroying Shader Programs..." );
	while ( m_shaderPrograms.size() > 0 )
	{
		delete m_shaderPrograms.back();
		m_shaderPrograms.back() = nullptr;
		m_shaderPrograms.pop_back();
	}

	WVDEBUG( "Destroying Textures..." );
	while ( m_textures.size() > 0 )
	{
		delete m_textures.back();
		m_textures.back() = nullptr;
		m_textures.pop_back();
	}

	WVDEBUG( "Unloaded all assets" );
}

void AssetManager::loadQueuedAssets( AssetManager* _instance )
{

	while ( _instance->m_loadQueue.size() > 0 )
	{
		LoadQueueObject current = _instance->m_loadQueue.front();
		
		switch ( current.assetType )
		{
		case(AssetType::MODEL):
			_instance->internalLoad<Model>( (Model**)current.out, current.path );
			break;
		case( AssetType::MESH ):
			_instance->internalLoad<Mesh>( (Mesh**)current.out, current.path );
			break;
		case( AssetType::MATERIAL ):
			_instance->internalLoad<Material>( (Material**)current.out, current.path );
			break;
		case( AssetType::SHADER_PROGRAM ):
			_instance->internalLoad<ShaderProgram>( (ShaderProgram**)current.out, current.path );
			break;
		case( AssetType::TEXTURE ):
			_instance->internalLoad<Texture>( (Texture**)current.out, current.path );
			break;
		}

		_instance->m_loadQueue.erase( _instance->m_loadQueue.begin() );
	}

	_instance->m_loadThreadMutex.lock();
	_instance->m_isLoading = false;
	_instance->m_loadThreadMutex.unlock();
}
