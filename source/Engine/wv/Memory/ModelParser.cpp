#include "ModelParser.h"

#include <wv/Application/Application.h>
#include <wv/Assets/Materials/Material.h>

#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>

#include <wv/Memory/MemoryDevice.h>
#include <fstream>

#ifdef EMSCRIPTEN
#define LOAD_WPR
#endif

#ifndef LOAD_WPR
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

struct Vertex
{
	wv::Vector3f position;
	wv::Vector3f normal;
	wv::Vector3f tangent;
	wv::Vector4f color;
	wv::Vector2f texCoord0;
};

#ifndef LOAD_WPR
std::string getAssimpMaterialTexturePath( aiMaterial* _material, aiTextureType _type, const std::string& _rootDir )
{
	aiString path;
	_material->GetTexture( _type, 0, &path );

	std::string fullPath( path.C_Str() );
	fullPath = _rootDir + "/" + fullPath;
	return fullPath;
}

void processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, wv::Mesh* _mesh )
{
	wv::GraphicsDevice* device = wv::Application::get()->device;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	
	// process vertices
	for ( unsigned int i = 0; i < _assimp_mesh->mNumVertices; i++ )
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

		for ( unsigned int j = 0; j < face.mNumIndices; j++ )
			indices.push_back( face.mIndices[ j ] );
	}

	if ( _assimp_mesh->mMaterialIndex >= 0 )
	{
		aiMaterial* assimpMaterial = _scene->mMaterials[ _assimp_mesh->mMaterialIndex ];
		
		/// TODO: generalize for any type of shader, material, or set of textures

		//mesh->material = getMaterial( "res/materials/mesh" );
		//

		//if ( assimpMaterial->GetTextureCount( aiTextureType_DIFFUSE ) )
		//{
		//	albedoPath = getAssimpMaterialTexturePath( assimpMaterial, aiTextureType_DIFFUSE, "res/textures" );
		//}
		//std::string mr_path = getAssimpMaterialTexturePath( assimp_material, aiTextureType_DIFFUSE_ROUGHNESS, _directory );
		//std::string normal_path = getAssimpMaterialTexturePath( assimp_material, aiTextureType_NORMALS, _directory );
		//
		//mesh->material->addTexture( "uAlbedo", albedo_path.c_str() );
		//mesh->material->addTexture( "uMetallicRoughness", mr_path.c_str() );
		//mesh->material->addTexture( "uNormal", normal_path.c_str() );

		//material = new wv::PhongMaterial( phongDesc );
		//material->create( device );

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
	layout.numElements = (unsigned int)elements.size();

	wv::PrimitiveDesc prDesc;
	prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
	prDesc.layout = &layout;

	prDesc.vertexBuffer     = vertices.data();
	prDesc.vertexBufferSize = (unsigned int)(vertices.size() * sizeof( Vertex ));
	prDesc.numVertices      = (unsigned int)vertices.size();

	prDesc.indexBuffer     = indices.data();
	prDesc.indexBufferSize = (unsigned int)(indices.size() * sizeof( unsigned int ));
	prDesc.numIndices      = (unsigned int)indices.size();

	wv::Primitive* primitive = device->createPrimitive( &prDesc, _mesh );
	//primitive->material = material;


	std::ofstream wprfile( _mesh->name + ".wpr", std::ios::binary );
	wprfile.write( (char*)&prDesc.numIndices, sizeof( unsigned int ) );
	wprfile.write( (char*)&prDesc.numVertices, sizeof( unsigned int ) );
	// data
	wprfile.write( (char*)indices.data(), prDesc.indexBufferSize );
	wprfile.write( (char*)vertices.data(), prDesc.vertexBufferSize);
	wprfile.close();
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
#endif

wv::Mesh* wv::assimp::Parser::load( const char* _path )
{
#ifndef LOAD_WPR
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
	mesh->name = _path;
	processAssimpNode( scene->mRootNode, scene, mesh );

#endif

#ifdef LOAD_WPR
	std::vector<wv::InputLayoutElement> elements = {
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
			{ 4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
			{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
	};
	wv::InputLayout layout;
	layout.elements = elements.data();
	layout.numElements = (unsigned int)elements.size();

	// custom mesh format export
	/*
	// data
	int pnumIndices = 0;
	int pnumVertices = 36;

	std::ofstream cubefile( "res/cube.wpr", std::ios::binary );
	// header
	cubefile.write( (char*)&pnumIndices, sizeof( int ) );
	cubefile.write( (char*)&pnumVertices, sizeof( int ) );
	// data
	cubefile.write( (char*)indices, sizeof( indices ) );
	cubefile.write( (char*)skyboxVertices, sizeof( skyboxVertices ) );
	cubefile.close();
	*/

	
	MemoryDevice mdevice;
	std::string path = std::string( _path ) + ".wpr";
	Memory mem = mdevice.loadMemory( path.c_str() );
	
	wv::PrimitiveDesc prDesc;
	{
		int numIndices = *reinterpret_cast<int*>( mem.data );
		int numVertices = *reinterpret_cast<int*>( mem.data + sizeof( int ) );
		int vertsSize = numVertices * sizeof( Vertex ); // 5 floats per vertex
		int indsSize = numIndices * sizeof( unsigned int );

		uint8_t* indexBuffer = mem.data + ( sizeof( int ) * 2 );
		uint8_t* vertexBuffer = indexBuffer + indsSize;

		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;

		prDesc.vertexBuffer = vertexBuffer;
		prDesc.vertexBufferSize = vertsSize;
		prDesc.numVertices = numVertices;

		prDesc.indexBuffer = indexBuffer;
		prDesc.indexBufferSize = indsSize;
		prDesc.numIndices = numIndices;
	}


	Application* app = Application::get();
	
	Mesh* mesh = app->device->createMesh( nullptr );
	
	Primitive* prm = app->device->createPrimitive( &prDesc, mesh );
	prm->material = new PhongMaterial();
	prm->material->create( app->device );

	mdevice.unloadMemory( &mem );
#endif
	return mesh;
}
