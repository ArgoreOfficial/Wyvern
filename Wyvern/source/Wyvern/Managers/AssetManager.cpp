#include "AssetManager.h"

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

void WV::AssetManager::internalLoadModel( Model** _out, const char* _meshPath, const char* _texturePath )
{
	WVDEBUG( "Creating Model...", _meshPath );

	Mesh* mesh = nullptr;
	AssetManager::load<Mesh>( &mesh, _meshPath );

	ShaderProgram* shaderProgram = nullptr;
	AssetManager::load<ShaderProgram>( &shaderProgram, "shaders/default.shader" );

	Texture* texture = nullptr;
	AssetManager::load<Texture>( &texture, _texturePath );

	Diffuse diffuse{ glm::vec4( 1.0f,1.0f,1.0f,1.0f ), texture };
	Material* material = new Material( shaderProgram, diffuse, glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

	Model* tmp = new Model( mesh, material );
	*_out = tmp;
}

void AssetManager::internalUnloadAll()
{
	clearMap<std::string, Mesh*>( m_meshes );
	clearMap<std::string, Material*>( m_materials );
	clearMap<std::string, ShaderProgram*>( m_shaderPrograms );
	clearMap<std::string, Texture*>( m_textures );
}

void WV::AssetManager::internalLoadQueued()
{
	while ( m_loadQueue.size() > 0 )
	{
		AssetQueueObject current = m_loadQueue.front();

		std::string filename = Filesystem::getFilenameFromPath( current.path );

		switch ( current.type )
		{
		case( Asset::AssetType::MESH ):
		{
			MeshAsset* meshAsset = new MeshAsset();
			meshAsset->load( current.path );
			m_meshAssets[ filename ] = meshAsset;
			
			break;
		}
		case( Asset::AssetType::TEXTURE ):
		{
			TextureAsset* textureAsset = new TextureAsset();
			textureAsset->load( current.path );
			m_textureAssets[ filename ] = textureAsset;

			break;
		}
		case( Asset::AssetType::SHADER ):
		{
			ShaderSource* shader = new ShaderSource( current.path );
			// meshAsset->load( current.path );
			m_shaderAssets[ filename ] = shader;

			break;
		}
		}

		m_loadQueue.erase( m_loadQueue.begin() );
	}
}

