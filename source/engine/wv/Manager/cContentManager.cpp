#include "cContentManager.h"

#include <cm/Core/stb_image.h>
#include <cm/Backends/iBackend.h>
#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>
#include <wv/Graphics/cMaterial.h>
#include <wv/Graphics/cShader.h>
#include <wv/Graphics/cModel.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

void wv::cContentManager::create()
{
}

std::string wv::cContentManager::loadFileToString( const std::string& _path )
{
	std::string line, text;
	std::ifstream in( _path );

	while ( std::getline( in, line ) )
	{
		text += line + "\n";
	}

	return text;
}

std::vector<std::string> wv::cContentManager::loadFileToVector( const std::string& _path )
{
	std::vector<std::string> lines;
	std::string line;
	std::ifstream in( _path );

	while ( std::getline( in, line ) )
		lines.push_back( line );
	
	return lines;
}

cm::Shader::hShaderProgram wv::cContentManager::loadShaderProgram( const std::string& _path )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	const std::string vert_path = _path + ".vert";
	const std::string frag_path = _path + ".frag";

	std::string vert = loadFileToString( vert_path );
	std::string frag = loadFileToString( frag_path );

	cm::Shader::sShader vert_shader = backend->createShader( vert.data(), cm::Shader::eShaderType::ShaderType_Vertex );
	cm::Shader::sShader frag_shader = backend->createShader( frag.data(), cm::Shader::eShaderType::ShaderType_Fragment );

	cm::Shader::hShaderProgram program = backend->createShaderProgram();
	backend->attachShader( program, vert_shader );
	backend->attachShader( program, frag_shader );
	backend->linkShaderProgram( program );

	return program;
}

cm::sTexture2D wv::cContentManager::loadTexture( const std::string& _path )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	
	cm::sTexture2D texture = backend->createTexture();
	unsigned char* data = stbi_load( _path.c_str(), &texture.width, &texture.height, &texture.num_channels, 0 );
	backend->generateTexture( texture, data );
	stbi_image_free( data );

	return texture;
}

wv::cMaterial* wv::cContentManager::loadMaterial( const std::string& _path )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	std::vector<std::string> file = loadFileToVector( _path + ".wmat" );
	std::map<std::string, std::string> material_values;

	for ( int i = 0; i < file.size(); i++ )
	{
		std::string line = file[ i ];

		line.erase( remove( line.begin(), line.end(), ' ' ), line.end() );

		std::stringstream stream( line );
		std::string segment;
		std::vector<std::string> seglist;
		while ( std::getline( stream, segment, ':' ) )
			seglist.push_back( segment );

		material_values[ seglist[ 0 ] ] = seglist[ 1 ];
	}

	cMaterial* mat = new cMaterial();
	mat->shader = loadShader( material_values["shader"] );
	
	for ( int i = 0; i < material_values.size() - 1; i++ )
	{
		cm::Shader::sShaderUniform uniform = backend->getUniform( mat->shader->shader_program_handle, i );
		if ( material_values.count( uniform.name ) == 0 )
			continue;
		
		if ( uniform.type == cm::Shader::ShaderUniformType_Sampler2D )
			mat->addTexture( uniform.name, material_values[ uniform.name ] );
	}

	return mat;
}

wv::cShader* wv::cContentManager::loadShader( const std::string& _path )
{
	wv::cShader* shader = new cShader();
	shader->shader_program_handle = loadShaderProgram( _path );

	return shader;
}

wv::cModel* wv::cContentManager::loadModel( const std::string& _path )
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( _path, aiProcess_Triangulate | aiProcess_FlipUVs );

	if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
	{
		printf( "ERROR::ASSIMP::%s\n", importer.GetErrorString() );
		return nullptr;
	}

	cModel* model = new cModel();
	model->path = _path;
	processAssimpNode( scene->mRootNode, scene, model );

	return model;
}

void wv::cContentManager::processAssimpNode( aiNode* _node, const aiScene* _scene, cModel* _model )
{
	std::string dir = _model->path.substr( 0, _model->path.find_last_of( '/' ) );

	// process all the node's meshes (if any)
	for ( unsigned int i = 0; i < _node->mNumMeshes; i++ )
	{
		aiMesh* mesh = _scene->mMeshes[ _node->mMeshes[ i ] ];
		_model->meshes.push_back( processAssimpMesh( mesh, _scene, dir ) );
	}

	// then do the same for each of its children
	for ( unsigned int i = 0; i < _node->mNumChildren; i++ )
	{
		processAssimpNode( _node->mChildren[ i ], _scene, _model );
	}
}

wv::cMesh* wv::cContentManager::processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, const std::string& _directory )
{
	cm::iBackend* backend = wv::cRenderer::getInstance().getBackend();
	std::vector<sVertex> vertices;
	std::vector<unsigned int> indices;
	
	wv::cMesh* mesh = new wv::cMesh();

	// process vertices
	for ( int i = 0; i < _assimp_mesh->mNumVertices; i++ )
	{
		sVertex v;
		v.position.x  = _assimp_mesh->mVertices[ i ].x;
		v.position.y  = _assimp_mesh->mVertices[ i ].y;
		v.position.z  = _assimp_mesh->mVertices[ i ].z;
		
		if ( _assimp_mesh->mTextureCoords[ 0 ] )
		{
			v.texCoord0.x = _assimp_mesh->mTextureCoords[ 0 ][ i ].x;
			v.texCoord0.y = _assimp_mesh->mTextureCoords[ 0 ][ i ].y;
		}
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

	// process material
	if ( _assimp_mesh->mMaterialIndex >= 0 )
	{
		aiMaterial* assimp_material = _scene->mMaterials[ _assimp_mesh->mMaterialIndex ];
		mesh->material = loadMaterial( "res/materials/mesh" );

		aiString texpath;
		assimp_material->GetTexture( aiTextureType_DIFFUSE, 0, &texpath );
		std::string fullpath( texpath.C_Str() );
		fullpath = _directory + "/" + fullpath;

		mesh->material->addTexture( "uAlbedo", fullpath.c_str() );
	}

	/* create vertex array */
	cm::hVertexArray vertex_array = backend->createVertexArray();
	backend->bindVertexArray( vertex_array );

	/* create vertex buffer */
	cm::sBuffer vertex_buffer = backend->createBuffer( cm::eBufferType::BufferType_Vertex );
	backend->bufferData( vertex_buffer, vertices.data(), sizeof( sVertex ) * vertices.size() );

	cm::sBuffer index_buffer = backend->createBuffer( cm::eBufferType::BufferType_Index );
	backend->bufferData( index_buffer, indices.data(), sizeof( unsigned int ) * indices.size() );

	cm::cVertexLayout layout;
	layout.push<float>( 3 );
	layout.push<float>( 2 );
	backend->bindVertexLayout( layout );

	backend->bindVertexArray( 0 );

	mesh->layout = layout;
	mesh->vertex_array = vertex_array;
	mesh->vertex_buffer = vertex_buffer;
	mesh->index_buffer = index_buffer;
	mesh->num_vertices = vertices.size();
	mesh->num_indices = indices.size();

	return mesh;
}

