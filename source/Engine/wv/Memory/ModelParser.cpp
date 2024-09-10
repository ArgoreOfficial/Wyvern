#include "ModelParser.h"

#include <wv/Engine/Engine.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Assets/Materials/MaterialRegistry.h>
#include <wv/Debug/Print.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Math/Triangle.h>
#include <wv/Memory/FileSystem.h>

#include <fstream>

#ifdef EMSCRIPTEN
//#define LOAD_WPR
#endif

#ifdef WV_SUPPORT_ASSIMP
#ifndef LOAD_WPR
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////

#ifndef LOAD_WPR
std::string getAssimpMaterialTexturePath( aiMaterial* _material, aiTextureType _type, const std::string& _rootDir )
{
	aiString path;
	_material->GetTexture( _type, 0, &path );

	std::string fullPath( path.C_Str() );
	fullPath = _rootDir + "/" + fullPath;
	return fullPath;
}

///////////////////////////////////////////////////////////////////////////////////////

void processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, wv::sMesh* _mesh, wv::cMaterialRegistry* _pMaterialRegistry )
{
	wv::iGraphicsDevice* device = wv::cEngine::get()->graphics;

	std::vector<wv::Vertex> vertices;
	std::vector<unsigned int> indices;
	
	// process vertices
	for ( unsigned int i = 0; i < _assimp_mesh->mNumVertices; i++ )
	{
		wv::Vertex v;
		v.position.x = _assimp_mesh->mVertices[ i ].x;
		v.position.y = _assimp_mesh->mVertices[ i ].y;
		v.position.z = _assimp_mesh->mVertices[ i ].z;

		if ( _assimp_mesh->HasNormals() )
		{
			v.normal.x = _assimp_mesh->mNormals[ i ].x;
			v.normal.y = _assimp_mesh->mNormals[ i ].y;
			v.normal.z = _assimp_mesh->mNormals[ i ].z;
		}

		if ( _assimp_mesh->HasVertexColors( i ) )
		{
			aiColor4D col = _assimp_mesh->mColors[ i ][ 1 ];
			v.color = { col.r, col.g, col.b, col.a };
		}
		else
			v.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		if ( _assimp_mesh->HasTangentsAndBitangents() )
		{
			v.tangent.x = _assimp_mesh->mTangents[ i ].x;
			v.tangent.y = _assimp_mesh->mTangents[ i ].y;
			v.tangent.z = _assimp_mesh->mTangents[ i ].z;
		}

		if ( _assimp_mesh->HasTextureCoords( 0 ) )
		{
			aiVector3D* texcoord = _assimp_mesh->mTextureCoords[ 0 ];
			v.texCoord0 = { texcoord[ i ].x, texcoord[ i ].y };
			
		}

		vertices.push_back( v );
	}

	// process indices
	for ( unsigned int i = 0; i < _assimp_mesh->mNumFaces; i++ )
	{
		aiFace face = _assimp_mesh->mFaces[ i ];

		if ( face.mNumIndices == 3 )
		{
			wv::Triangle3f triangle{
				vertices[ face.mIndices[ 0 ] ].position,
				vertices[ face.mIndices[ 1 ] ].position,
				vertices[ face.mIndices[ 2 ] ].position
			};
			_mesh->triangles.push_back( triangle );
		}
		
		for ( unsigned int j = 0; j < face.mNumIndices; j++ )
			indices.push_back( face.mIndices[ j ] );
	}

	std::vector<wv::sVertexAttribute> elements = {
			{ "a_Pos",       3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
			{ "a_Normal",    3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
			{ "a_Tangent",   3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
			{ "a_Color",     4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
			{ "a_TexCoord0", 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
	};
	wv::sVertexLayout layout;
	layout.elements = elements.data();
	layout.numElements = (unsigned int)elements.size();

	wv::PrimitiveDesc prDesc;
	prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
	prDesc.layout = &layout;

	prDesc.vertices = vertices.data();
	prDesc.sizeVertices = vertices.size() * sizeof( wv::Vertex );
	prDesc.numIndices = indices.size();
	prDesc.indices32 = indices.data();
	
	wv::Primitive* primitive = device->createPrimitive( &prDesc );
	primitive->vertices = vertices;
	primitive->indices = indices;

	wv::cFileSystem filesystem;

	if ( _assimp_mesh->mMaterialIndex >= 0 )
	{
		aiMaterial* assimpMaterial = _scene->mMaterials[ _assimp_mesh->mMaterialIndex ];
		
		wv::cFileSystem md;
		std::string matPath = assimpMaterial->GetName().C_Str();
		wv::cMaterial* material = nullptr;

		/*
		if( md.fileExists( matPath ) ) 
		{
			material = _pMaterialRegistry->loadMaterial( assimpMaterial->GetName().C_Str() );
		}
		else
		{
			material = _pMaterialRegistry->loadMaterial( "phong" );
		}
		*/
		material = _pMaterialRegistry->loadMaterial( "phong" );
		
		primitive->material = material;
	}

	_mesh->primitives.push_back( primitive );
}

///////////////////////////////////////////////////////////////////////////////////////

void processAssimpNode( aiNode* _node, const aiScene* _scene, wv::sMeshNode* _meshNode, wv::iGraphicsDevice* _pGraphicsDevice, wv::cMaterialRegistry* _pMaterialRegistry )
{
	// std::string dir = _mesh->path.substr( 0, _mesh->path.find_last_of( '/' ) );
	
	aiVector3D pos, scale, rot;
	_node->mTransformation.Decompose( scale, rot, pos );

	_meshNode->name = std::string( _node->mName.C_Str() );
	_meshNode->transform.position = { pos.x, pos.y, pos.z };
	_meshNode->transform.scale    = { scale.x, scale.y, scale.z };
	_meshNode->transform.rotation = { 
		wv::Math::degrees( rot.x ), 
		wv::Math::degrees( rot.y ), 
		wv::Math::degrees( rot.z ) 
	};

	// process all the node's meshes (if any)
	for ( unsigned int i = 0; i < _node->mNumMeshes; i++ )
	{
		wv::sMesh* mesh = _pGraphicsDevice->createMesh( nullptr );

		aiMesh* aimesh = _scene->mMeshes[ _node->mMeshes[ i ] ];
		processAssimpMesh( aimesh, _scene, mesh, _pMaterialRegistry );
		
		_meshNode->transform.addChild( &mesh->transform );
		_meshNode->meshes.push_back( mesh );
	}

	// then do the same for each of its children
	for( unsigned int i = 0; i < _node->mNumChildren; i++ )
	{
		wv::sMeshNode* meshNode = new wv::sMeshNode();
		processAssimpNode( _node->mChildren[ i ], _scene, meshNode, _pGraphicsDevice, _pMaterialRegistry );

		_meshNode->transform.addChild( &meshNode->transform );
		_meshNode->children.push_back( meshNode );
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////

wv::sMeshNode* wv::Parser::load( const char* _path, wv::cMaterialRegistry* _pMaterialRegistry )
{
	cFileSystem md;
	std::string path = std::string( _path ) + ".dae";
	Memory* meshMem = md.loadMemory( path );
	
	if ( !meshMem )
		return nullptr;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory( meshMem->data, meshMem->size, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace );
	
	md.unloadMemory( meshMem );


	// TODO: change to wv::assert
	if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "ASSIMP::%s\n", importer.GetErrorString() );
		return nullptr;
	}

	wv::iGraphicsDevice* device = wv::cEngine::get()->graphics;
	
	wv::sMeshNode* mesh = new wv::sMeshNode();
	processAssimpNode( scene->mRootNode, scene, mesh, device, _pMaterialRegistry );
	
	return mesh;
}
