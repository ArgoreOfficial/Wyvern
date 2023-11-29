#include "cModel.h"

#include <Wyvern/cApplication.h>
#include <Wyvern/Filesystem/cFilesystem.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Managers/cAssetManager.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace wv;

cModel::cModel( std::string _path ) : iAsset( _path )
{
	//cAssetManager::addAssetToLoadQueue( this );
}

cModel::~cModel( void )
{
	// destroy meshes
}

void wv::cModel::load( void )
{

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( m_path.c_str(), aiProcess_Triangulate );

	if ( !scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
	{
		WV_ERROR( "%s", importer.GetErrorString() );
	}

	for ( size_t i = 0; i < scene->mNumMeshes; i++ )
	{
		aiMesh* mesh = scene->mMeshes[ i ];
		cMesh* newMesh = new cMesh();

		for ( size_t vert = 0; vert < mesh->mNumVertices; vert++ )
		{

			newMesh->vertices.push_back( mesh->mVertices[ vert ].x );
			newMesh->vertices.push_back( mesh->mVertices[ vert ].y );
			newMesh->vertices.push_back( mesh->mVertices[ vert ].z );

			newMesh->vertices.push_back( mesh->mTextureCoords[0][vert].x );
			newMesh->vertices.push_back( mesh->mTextureCoords[0][vert].y );
			
			newMesh->m_vertex_count += 3;

		}

		for ( size_t face = 0; face < mesh->mNumFaces; face++ )
		{
			aiFace& faceref = mesh->mFaces[ face ];
			for ( size_t indice = 0; indice < faceref.mNumIndices; indice++ )
			{
				newMesh->indices.push_back( faceref.mIndices[indice] );
			}
		}
		
		newMesh->create();

		m_meshes.push_back( newMesh );
	}

	m_material.load();

}

void cModel::render( void )
{
	glm::mat4 model = glm::mat4( 1.0f );

	m_material.getShader().setUniform4f( "model",      model );
	m_material.getShader().setUniform4f( "view",       cApplication::getViewport().getActiveCamera()->getViewMatrix() );
	m_material.getShader().setUniform4f( "projection", cApplication::getViewport().getActiveCamera()->getProjMatrix() );
	m_material.use();

	for ( size_t i = 0; i < m_meshes.size(); i++ )
	{
		m_meshes[ i ]->render();
	}
}
