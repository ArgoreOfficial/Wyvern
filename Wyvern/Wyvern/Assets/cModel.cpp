#include "cModel.h"

#include <Wyvern/Filesystem/cFilesystem.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Managers/cAssetManager.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace wv;

cModel::cModel( std::string _path ) : iAsset( _path )
{
	cAssetManager::addAssetToLoadQueue( this );
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

		}

		for ( size_t face = 0; face < mesh->mNumFaces; face++ )
		{
			aiFace& faceref = mesh->mFaces[ face ];
			for ( size_t indice = 0; indice < faceref.mNumIndices; indice++ )
			{
				newMesh->indices.push_back( faceref.mIndices[indice] );
			}
		}
		
		m_meshes.push_back( newMesh );
		m_meshes.back()->create();
	}

}

void cModel::render( void )
{
	for ( size_t i = 0; i < m_meshes.size(); i++ )
	{
		m_meshes[ i ]->render();
	}
}
