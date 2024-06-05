#include "ModelParser.h"

#include <wv/Application/Application.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

wv::gltf::Parser::Parser()
{
}

struct Vertex
{
	wv::Vector3f position;
	wv::Vector3f normal;
	wv::Vector3f tangent;
	wv::Vector4f color;
	wv::Vector2f texCoord0;
};

void processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, wv::Mesh* _mesh )
{
	
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// process vertices
	for ( int i = 0; i < _assimp_mesh->mNumVertices; i++ )
	{
		Vertex v;
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

		aiVector3D* texcoord = _assimp_mesh->mTextureCoords[ 0 ];
		if ( texcoord )
			v.texCoord0 = { texcoord[ i ].x, texcoord[ i ].y };
		else
			v.texCoord0 = { 0.0f, 0.0f };

		vertices.push_back( v );
	}

	// process indices
	for ( unsigned int i = 0; i < _assimp_mesh->mNumFaces; i++ )
	{
		aiFace face = _assimp_mesh->mFaces[ i ];

		for ( int j = 0; j < face.mNumIndices; j++ )
			indices.push_back( face.mIndices[ j ] );
	}

	/// TODO: process material
	if ( _assimp_mesh->mMaterialIndex >= 0 )
	{
		aiMaterial* assimp_material = _scene->mMaterials[ _assimp_mesh->mMaterialIndex ];
		//mesh->material = getMaterial( "res/materials/mesh" );
		//
		//std::string albedo_path = getAssimpMaterialTexturePath( assimp_material, aiTextureType_DIFFUSE, _directory );
		//std::string mr_path = getAssimpMaterialTexturePath( assimp_material, aiTextureType_DIFFUSE_ROUGHNESS, _directory );
		//std::string normal_path = getAssimpMaterialTexturePath( assimp_material, aiTextureType_NORMALS, _directory );
		//
		//mesh->material->addTexture( "uAlbedo", albedo_path.c_str() );
		//mesh->material->addTexture( "uMetallicRoughness", mr_path.c_str() );
		//mesh->material->addTexture( "uNormal", normal_path.c_str() );
	}

	std::vector<wv::InputLayoutElement> elements = {
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
			{ 4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
			{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
	};
	wv::InputLayout layout;
	layout.elements = elements.data();
	layout.numElements = elements.size();

	wv::PrimitiveDesc prDesc;
	prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
	prDesc.layout = &layout;

	prDesc.vertexBuffer     = vertices.data();
	prDesc.vertexBufferSize = vertices.size() * sizeof( Vertex );
	prDesc.numVertices      = vertices.size();

	prDesc.indexBuffer     = indices.data();
	prDesc.indexBufferSize = indices.size() * sizeof( unsigned int );
	prDesc.numIndices      = indices.size();

	wv::GraphicsDevice* device = wv::Application::get()->device;
	wv::Primitive* primitive = device->createPrimitive( &prDesc, _mesh );
}

void processAssimpNode( aiNode* _node, const aiScene* _scene, wv::Mesh* _mesh )
{
	// std::string dir = _mesh->path.substr( 0, _mesh->path.find_last_of( '/' ) );

	// process all the node's meshes (if any)
	for ( unsigned int i = 0; i < _node->mNumMeshes; i++ )
	{
		aiMesh* mesh = _scene->mMeshes[ _node->mMeshes[ i ] ];
		processAssimpMesh( mesh, _scene, _mesh );
	}

	// then do the same for each of its children
	for ( unsigned int i = 0; i < _node->mNumChildren; i++ )
		processAssimpNode( _node->mChildren[ i ], _scene, _mesh );

}

wv::Mesh* wv::assimp::Parser::load( const char* _path )
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( _path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );

	// TODO: change to wv::assert
	if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
	{
		printf( "ERROR::ASSIMP::%s\n", importer.GetErrorString() );
		return nullptr;
	}

	wv::GraphicsDevice* device = wv::Application::get()->device;
	Mesh* mesh = device->createMesh( nullptr );
	processAssimpNode( scene->mRootNode, scene, mesh );

	return mesh;
}
