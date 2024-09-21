#include "ModelParser.h"

#include <wv/Engine/Engine.h>
#include <wv/Material/Material.h>
#include <wv/Debug/Print.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Math/Triangle.h>
#include <wv/Memory/FileSystem.h>

#include <wv/Resource/ResourceRegistry.h>

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

void processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, wv::sMesh** _outMesh, wv::sMeshNode* _meshNode, wv::cResourceRegistry* _pResourceRegistry )
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
		
		for ( unsigned int j = 0; j < face.mNumIndices; j++ )
			indices.push_back( face.mIndices[ j ] );
	}

	/// this reeeeeeeally needs to be reworked

	wv::cMaterial* material = nullptr;
	if ( _assimp_mesh->mMaterialIndex >= 0 )
	{
		aiMaterial* assimpMaterial = _scene->mMaterials[ _assimp_mesh->mMaterialIndex ];

		wv::cFileSystem& md = *wv::cEngine::get()->m_pFileSystem;

		std::string materialName = assimpMaterial->GetName().C_Str();

		if ( materialName == "" )  // fallback to DefaultMaterial
			materialName = "DefaultMaterial";

		if ( md.getFullPath( materialName + ".wmat" ) == "" )
		{
			wv::Debug::Print( wv::Debug::WV_PRINT_WARN, "Material %s does not exist. Please create it\n", materialName );
			materialName = "DefaultMaterial";
			// create new material file
		}

		material = _pResourceRegistry->load<wv::cMaterial>( materialName + ".wmat" );
	}

	{ // create primitive
		std::vector<wv::sVertexAttribute> elements = {
				{ "a_Pos",       3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
				{ "a_Normal",    3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
				{ "a_Tangent",   3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
				{ "a_Color",     4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
				{ "a_TexCoord0", 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
		};
		wv::sVertexLayout layout;
		layout.numElements = (unsigned int)elements.size();
		layout.elements = new wv::sVertexAttribute[ elements.size() ];
		memcpy( layout.elements, elements.data(), elements.size() * sizeof( wv::sVertexAttribute ) );

		wv::sMeshDesc prDesc;
		prDesc.layout = layout;
		prDesc.pParentTransform = &_meshNode->transform;

		size_t sizeVertices = vertices.size() * sizeof( wv::Vertex );
		prDesc.sizeVertices = sizeVertices;
		
		prDesc.vertices = new wv::Vertex[ vertices.size() ];
		memcpy( prDesc.vertices, vertices.data(), sizeVertices );
		
		prDesc.numIndices = indices.size();
		prDesc.indices32 = new uint32_t[ indices.size() ];
		memcpy( prDesc.indices32, indices.data(), indices.size() * sizeof( uint32_t ) );
		
		prDesc.pMaterial = material;
		
		// buffer
		uint32_t cmdBuffer = device->getCommandBuffer();
		device->bufferCommand( cmdBuffer, wv::WV_GPUTASK_CREATE_MESH, _outMesh, &prDesc );
		device->submitCommandBuffer( cmdBuffer );

		if ( device->getThreadID() == std::this_thread::get_id() )
			device->executeCommandBuffer( cmdBuffer );
	}

	wv::cFileSystem filesystem;
}

///////////////////////////////////////////////////////////////////////////////////////

void processAssimpNode( aiNode* _node, const aiScene* _scene, wv::sMeshNode* _meshNode, wv::iGraphicsDevice* _pGraphicsDevice, wv::cResourceRegistry* _pResourceRegistry )
{
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
		_meshNode->meshes.resize( _node->mNumMeshes );

		aiMesh* aimesh = _scene->mMeshes[ _node->mMeshes[ i ] ];
		processAssimpMesh( aimesh, _scene, &_meshNode->meshes[ i ], _meshNode, _pResourceRegistry );
	}

	// then do the same for each of its children
	for( unsigned int i = 0; i < _node->mNumChildren; i++ )
	{
		wv::sMeshNode* meshNode = new wv::sMeshNode();
		processAssimpNode( _node->mChildren[ i ], _scene, meshNode, _pGraphicsDevice, _pResourceRegistry );

		_meshNode->transform.addChild( &meshNode->transform );
		_meshNode->children.push_back( meshNode );
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////

wv::sMeshNode* wv::Parser::load( const char* _path, wv::cResourceRegistry* _pResourceRegistry )
{
	cFileSystem md;
	std::string path = std::string( _path );
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
	processAssimpNode( scene->mRootNode, scene, mesh, device, _pResourceRegistry );
	
	return mesh;
}
