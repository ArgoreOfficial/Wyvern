#include "ModelParser.h"

#include <wv/Engine/Engine.h>
#include <wv/Material/Material.h>
#include <wv/Debug/Print.h>
#include <wv/Graphics/Graphics.h>
#include <wv/Mesh/MeshResource.h>
#include <wv/Math/Triangle.h>
#include <wv/Memory/FileSystem.h>

#include <wv/Texture/TextureResource.h>
#include <wv/Resource/ResourceRegistry.h>

#include <wv/Auxiliary/json/json11.hpp>

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

#ifdef WV_SUPPORT_ASSIMP
std::string getAssimpMaterialTexturePath( aiMaterial* _material, aiTextureType _type, const std::string& _rootDir )
{
	aiString path;
	_material->GetTexture( _type, 0, &path );

	std::string fullPath( path.C_Str() );
	fullPath = _rootDir + "/" + fullPath;
	return fullPath;
}

///////////////////////////////////////////////////////////////////////////////////////

void processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, wv::MeshID* _outMesh, wv::sMeshNode* _meshNode, wv::cResourceRegistry* _pResourceRegistry )
{
	wv::iLowLevelGraphics* device = wv::cEngine::get()->graphics;

	std::vector<wv::Vertex> vertices;
	std::vector<unsigned int> indices;

	// process vertices
	for( unsigned int i = 0; i < _assimp_mesh->mNumVertices; i++ )
	{
		wv::Vertex v;
		v.position.x = _assimp_mesh->mVertices[ i ].x;
		v.position.y = _assimp_mesh->mVertices[ i ].y;
		v.position.z = _assimp_mesh->mVertices[ i ].z;

		if( _assimp_mesh->HasNormals() )
		{
			v.normal.x = _assimp_mesh->mNormals[ i ].x;
			v.normal.y = _assimp_mesh->mNormals[ i ].y;
			v.normal.z = _assimp_mesh->mNormals[ i ].z;
		}

		if( _assimp_mesh->HasVertexColors( i ) )
		{
			aiColor4D col = _assimp_mesh->mColors[ i ][ 1 ];
			v.color = { col.r, col.g, col.b, col.a };
		}
		else
			v.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		if( _assimp_mesh->HasTangentsAndBitangents() )
		{
			v.tangent.x = _assimp_mesh->mTangents[ i ].x;
			v.tangent.y = _assimp_mesh->mTangents[ i ].y;
			v.tangent.z = _assimp_mesh->mTangents[ i ].z;
		}

		if( _assimp_mesh->HasTextureCoords( 0 ) )
		{
			aiVector3D* texcoord = _assimp_mesh->mTextureCoords[ 0 ];
			v.texCoord0 = { texcoord[ i ].x, texcoord[ i ].y };
		}

		vertices.push_back( v );
	}

	// process indices
	for( unsigned int i = 0; i < _assimp_mesh->mNumFaces; i++ )
	{
		aiFace face = _assimp_mesh->mFaces[ i ];

		/*
		if ( face.mNumIndices == 3 )
		{
			wv::Triangle3f triangle{
				vertices[ face.mIndices[ 0 ] ].position,
				vertices[ face.mIndices[ 1 ] ].position,
				vertices[ face.mIndices[ 2 ] ].position
			};
			_mesh->triangles.push_back( triangle );
		}
		*/

		for( unsigned int j = 0; j < face.mNumIndices; j++ )
			indices.push_back( face.mIndices[ j ] );
	}

	/// this reeeeeeeally needs to be reworked

	wv::cMaterial* material = nullptr;
	if( _assimp_mesh->mMaterialIndex >= 0 )
	{
		aiMaterial* assimpMaterial = _scene->mMaterials[ _assimp_mesh->mMaterialIndex ];

		wv::cFileSystem& md = *wv::cEngine::get()->m_pFileSystem;

		std::string materialName = assimpMaterial->GetName().C_Str();

		if( materialName == "" ) // fallback to DefaultMaterial
			materialName = "DefaultMaterial";

		if( md.getFullPath( materialName + ".wmat" ) == "" )
		{
			wv::Debug::Print( wv::Debug::WV_PRINT_WARN, "Material %s does not exist. One \n", materialName.c_str() );
			// materialName = "DefaultMaterial";

			aiString diffusePath;
			assimpMaterial->GetTexture( aiTextureType_DIFFUSE, 0, &diffusePath );

			json11::Json newMaterial = json11::Json::object{
				{ "shader", "basic" },
				{ "textures", json11::Json::array 
					{
						json11::Json::object {
							{ "name", "u_Albedo" },
							{ "texture", diffusePath.C_Str() },
							{ "filtering", wv::eTextureFiltering::WV_TEXTURE_FILTER_LINEAR }
						}
					} 
				}
			};
			

			std::ofstream file( "materials/" + materialName + ".wmat" );
			file << newMaterial.dump();
			file.close();

			// create new material file
		}

		material = _pResourceRegistry->load<wv::cMaterial>( materialName + ".wmat" );
	}

	{ // create primitive
		wv::sMeshDesc prDesc;
		prDesc.pParentTransform = &_meshNode->transform;

		size_t sizeVertices = vertices.size() * sizeof( wv::Vertex );
		prDesc.sizeVertices = sizeVertices;

		prDesc.vertices = WV_NEW_ARR( uint8_t, sizeVertices );
		memcpy( prDesc.vertices, vertices.data(), sizeVertices );

		prDesc.numIndices = indices.size();
		prDesc.pIndices32 = WV_NEW_ARR( uint32_t, indices.size() );
		memcpy( prDesc.pIndices32, indices.data(), indices.size() * sizeof( uint32_t ) );

		prDesc.pMaterial = material;
		prDesc.deleteData = true;

		// buffer
		*_outMesh = device->createMesh( prDesc );
	}

	wv::cFileSystem filesystem;
}

///////////////////////////////////////////////////////////////////////////////////////

void processAssimpNode( aiNode* _node, const aiScene* _scene, wv::sMeshNode* _meshNode, wv::iLowLevelGraphics* _pLowLevelGraphics, wv::cResourceRegistry* _pResourceRegistry )
{
	aiVector3D pos, scale, rot;
	_node->mTransformation.Decompose( scale, rot, pos );

	_meshNode->name = std::string( _node->mName.C_Str() );
	_meshNode->transform.position = { pos.x, pos.y, pos.z };
	_meshNode->transform.scale = { scale.x, scale.y, scale.z };
	_meshNode->transform.rotation = {
		wv::Math::degrees( rot.x ),
		wv::Math::degrees( rot.y ),
		wv::Math::degrees( rot.z ) };

	_meshNode->transform.update( _meshNode->transform.pParent );

	// process all the node's meshes (if any)
	_meshNode->meshes.assign( _node->mNumMeshes, { 0 } );
	for( unsigned int i = 0; i < _node->mNumMeshes; i++ )
	{
		aiMesh* aimesh = _scene->mMeshes[ _node->mMeshes[ i ] ];
		processAssimpMesh( aimesh, _scene, &_meshNode->meshes[ i ], _meshNode, _pResourceRegistry );

	}

	// then do the same for each of its children
	for( unsigned int i = 0; i < _node->mNumChildren; i++ )
	{
		wv::sMeshNode* meshNode = WV_NEW( wv::sMeshNode );
		_meshNode->transform.addChild( &meshNode->transform );
		_meshNode->children.push_back( meshNode );

		processAssimpNode( _node->mChildren[ i ], _scene, meshNode, _pLowLevelGraphics, _pResourceRegistry );
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////

wv::sMeshNode* wv::Parser::load( const char* _path, wv::cResourceRegistry* _pResourceRegistry )
{
#ifdef WV_SUPPORT_ASSIMP
	cFileSystem md;
	std::string path = std::string( _path );
	Memory* meshMem = md.loadMemory( path );

	if( !meshMem )
		return nullptr;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory( meshMem->data, meshMem->size, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals );

	md.unloadMemory( meshMem );

	// TODO: change to wv::assert
	if( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "ASSIMP::%s\n", importer.GetErrorString() );
		return nullptr;
	}

	wv::iLowLevelGraphics* device = wv::cEngine::get()->graphics;

	wv::sMeshNode* mesh = WV_NEW( wv::sMeshNode );
	processAssimpNode( scene->mRootNode, scene, mesh, device, _pResourceRegistry );
	
	return mesh;
#else
	return nullptr;
#endif
}
