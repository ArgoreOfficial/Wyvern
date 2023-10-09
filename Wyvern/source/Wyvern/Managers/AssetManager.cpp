#include "AssetManager.h"
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Filesystem/Filesystem.h>
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Rendering/Framework/RenderObject.h>

using namespace WV;

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

Mesh* AssetManager::internalLoadMesh( const char* _path )
{

	if ( !Filesystem::fileExists( _path, true ) ) return nullptr; // check if mesh file exists

	objl::Loader loader;
	loader.LoadFile( _path );

	if ( loader.LoadedMeshes.size() == 0 || loader.LoadedVertices.size() == 0 || loader.LoadedIndices.size() == 0 )
	{ // file didn't load properly or is empty
		WVERROR( "Could not load mesh %s", _path );
		return nullptr;
	}

	WVDEBUG( "Loaded mesh %s", loader.LoadedMeshes[ 0 ].MeshName.c_str() );
	WVDEBUG( "Vertices: %i, Faces: %i", loader.LoadedVertices.size(), loader.LoadedIndices.size() / 3 );

	std::vector<unsigned int> indices;
	std::vector<float> vertices;

	for ( int i = 0; i < loader.LoadedVertices.size(); i++ )
	{
		vertices.push_back( loader.LoadedVertices[ i ].Position.X );
		vertices.push_back( loader.LoadedVertices[ i ].Position.Y );
		vertices.push_back( loader.LoadedVertices[ i ].Position.Z );
		vertices.push_back( loader.LoadedVertices[ i ].TextureCoordinate.X );
		vertices.push_back( loader.LoadedVertices[ i ].TextureCoordinate.Y );
	}
	for ( int i = 0; i < loader.LoadedIndices.size(); i++ )
	{
		indices.push_back( loader.LoadedIndices[ i ] );
	}

	VertexArray* vertexArray = new VertexArray;
	VertexBuffer* vertexBuffer = new VertexBuffer( &vertices[ 0 ], vertices.size() * sizeof( float ) ); // 3 values(xyz) * sizeof(float)

	VertexBufferLayout layout;
	layout.Push<float>( 3 ); // pos
	layout.Push<float>( 2 ); // uv
	vertexArray->AddBuffer( *vertexBuffer, layout );

	IndexBuffer* indexBuffer = new IndexBuffer( &indices[ 0 ], indices.size() );

	Mesh* mesh = new Mesh{ vertexArray, indexBuffer };
	m_meshes.push_back( mesh );

	return mesh;
}

Material* AssetManager::internalLoadMaterial( const char* _path )
{

	return nullptr;
}

ShaderProgram* AssetManager::internalLoadShader( const char* _path )
{

	if ( !Filesystem::fileExists( _path, true ) ) return nullptr; // check if shader file exists

	ShaderSource shaderSource( _path );
	ShaderProgram* shaderProgram = new ShaderProgram( shaderSource );

	m_shaderPrograms.push_back( shaderProgram );
	return shaderProgram;
}

Texture* AssetManager::internalLoadTexture( const char* _path )
{
	if ( !Filesystem::fileExists( _path, true ) ) return nullptr; // check if texture file exists

	Texture* texture = new Texture( _path );
	texture->Bind();
	// shaderProgram->SetUniform1i( "u_Texture", 0 );

	m_textures.push_back( texture );
	return texture;
}
