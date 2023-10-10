#include "AssetManager.h"

using namespace WV;

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
	while ( m_meshes.size() > 0 ) 
	{ 
		delete m_meshes.back(); 
		m_meshes.back() = nullptr;
		m_meshes.pop_back(); 
	}
	
	while ( m_materials.size() > 0 )
	{
		delete m_materials.back();
		m_materials.back() = nullptr;
		m_materials.pop_back();
	}
	
	while ( m_shaderPrograms.size() > 0 )
	{
		delete m_shaderPrograms.back();
		m_shaderPrograms.back() = nullptr;
		m_shaderPrograms.pop_back();
	}

	while ( m_textures.size() > 0 )
	{
		delete m_textures.back();
		m_textures.back() = nullptr;
		m_textures.pop_back();
	}
}

