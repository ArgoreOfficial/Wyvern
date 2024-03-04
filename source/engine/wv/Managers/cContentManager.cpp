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


#define STB_IMAGE_IMPLEMENTATION
#include <cm/Core/stb_image.h>


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

cm::sTexture2D* wv::cContentManager::getTexture( const std::string& _path, bool _ignore_existing )
{
	std::string name = getFilenameFromPath( _path );

	if ( m_textures.count( name ) )
		return m_textures[ name ];

	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	
	cm::sTexture2D* texture = new cm::sTexture2D();
	*texture = backend->createTexture();

	unsigned char* data = stbi_load( _path.c_str(), &texture->width, &texture->height, &texture->num_channels, 0 );
	backend->generateTexture( *texture, data );
	stbi_image_free( data );

	if ( texture->handle == 0 )
	{
		printf( "Failed to load texture\n" ); // TODO: change wv::log
		delete texture;
		return nullptr;
	}

	printf( "Loaded texture %s\n", name.c_str() ); // TODO: change wv::log
	m_textures[ name ] = texture;

	return texture;
}

wv::cMaterial* wv::cContentManager::getMaterial( const std::string& _path, bool _ignore_existing )
{
	//std::string name = getFilenameFromPath( _path );

	//if ( m_materials.count( name ) )
	//	return m_materials[ name ];

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
		
		if ( seglist.size() != 2 )
			continue;

		if ( seglist[ 0 ] == "" || seglist[ 1 ] == "" )
			continue;
		
		material_values[ seglist[ 0 ] ] = seglist[ 1 ];
	}

	cMaterial* mat = new cMaterial(); // TODO: keep track and destroy
	mat->shader = getShader( material_values[ "shader" ] );
	
	int loc = -1;
	int uniform_index = 0;
	do
	{
		cm::Shader::sUniform uniform = backend->getUniform( mat->shader->shader_program_handle, uniform_index );
		loc = uniform.location;
		uniform_index++;
		
		if ( material_values.count( uniform.name ) == 0 )
			continue;
		
		if ( uniform.type == cm::Shader::ShaderUniformType_Sampler2D )
			mat->addTexture( uniform.name, material_values[ uniform.name ] );
		
	} while ( loc != -1 );

	//m_materials[ name ] = mat;
	m_material_indices.push_back( mat );

	return mat;
}

wv::cMaterial* wv::cContentManager::getMaterial( int _index )
{
	if( _index < 0 || _index > m_material_indices.size() )
		return nullptr;

	return m_material_indices[ _index ];
}

void wv::cContentManager::bindMaterial( int _index )
{
	if ( _index == m_currently_bound_material || _index < 0 || _index > m_material_indices.size() )
		return;

	m_material_indices[ _index ]->bind();
	m_currently_bound_material = _index;
}

void wv::cContentManager::unbindMaterial()
{
	m_material_indices[ m_currently_bound_material ]->unbind();
	m_currently_bound_material = -1;
}

wv::cShader* wv::cContentManager::getShader( const std::string& _path, bool _ignore_existing )
{
	std::string name = getFilenameFromPath( _path );
	wv::cShader* shader = nullptr;

	if ( m_shaders.count( name ) != 0 )
	{
		if ( _ignore_existing )
			shader = m_shaders[ name ];
		else
			return m_shaders[ name ];
		
	}
	else
	{
		shader = new cShader( name, _path );
		printf( "Creating shader %s\n", name.c_str() ); // TODO: change wv::log
	}

	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	const std::string vert_path = _path + ".vert";
	const std::string frag_path = _path + ".frag";

	std::string vert = loadFileToString( vert_path );
	std::string frag = loadFileToString( frag_path );

	cm::Shader::sShader vert_shader = backend->createShader( vert, cm::Shader::eShaderType::ShaderType_Vertex );
	cm::Shader::sShader frag_shader = backend->createShader( frag, cm::Shader::eShaderType::ShaderType_Fragment );
	
	cm::Shader::hShaderProgram program = backend->createShaderProgram();
	backend->attachShader( program, vert_shader );
	backend->attachShader( program, frag_shader );
	backend->linkShaderProgram( program );
	
	shader->shader_program_handle = program;
	shader->createUniformBlock();
	m_shaders[ name ] = shader;

	backend->destroyShader( vert_shader );
	backend->destroyShader( frag_shader );

	return shader;
}

wv::cModel* wv::cContentManager::getModel( const std::string& _path, bool _ignore_existing )
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( _path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );

	// TODO: change to wv::assert
	if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
	{
		printf( "ERROR::ASSIMP::%s\n", importer.GetErrorString() );
		return nullptr;
	}

	cModel* model = new cModel(); // TODO: keep track and destroy
	model->path = _path;
	processAssimpNode( scene->mRootNode, scene, model );
	
	return model;
}

void wv::cContentManager::destroyShader( cShader* _shader )
{
	if ( m_shaders.count( _shader->name ) )
	{
		m_shaders.erase( _shader->name );
		delete _shader;
	}
}

void wv::cContentManager::destroyTexture( cm::sTexture2D* _texture )
{

	if ( m_shaders.count( _texture->name ) )
	{
		cRenderer::getInstance().getBackend()->destroyTexture( *_texture );
		m_shaders.erase( _texture->name );
		delete _texture;
	}
}

std::string wv::cContentManager::getFilenameFromPath( const std::string& _path )
{
	return _path.substr( _path.find_last_of( "/\\" ) + 1 );
}

void wv::cContentManager::reloadAllShaders()
{
	m_uniform_blocks = 0; // reset uniform block count, used for binding uniform buffer objects

	for ( auto& shader : m_shaders )
		shader.second->destroy();

	for ( auto& shader : m_shaders )
		shader.second = getShader( shader.second->path, true );

	printf( "Reloaded shaders\n" ); // TODO: change to wv::log
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
		processAssimpNode( _node->mChildren[ i ], _scene, _model );
	
}

wv::cMesh* wv::cContentManager::processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, const std::string& _directory )
{
	cm::iBackend* backend = wv::cRenderer::getInstance().getBackend();
	std::vector<sVertex> vertices;
	std::vector<unsigned int> indices;
	
	wv::cMesh* mesh = new wv::cMesh(); // TODO: keep track and destroy

	// process vertices
	for ( int i = 0; i < _assimp_mesh->mNumVertices; i++ )
	{
		sVertex v;
		v.position.x  = _assimp_mesh->mVertices[ i ].x;
		v.position.y  = _assimp_mesh->mVertices[ i ].y;
		v.position.z  = _assimp_mesh->mVertices[ i ].z;
		
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
			v.tex_coord_0 = { texcoord[ i ].x, texcoord[ i ].y };
		else
			v.tex_coord_0 = { 0.0f, 0.0f };

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
		mesh->material = getMaterial( "res/materials/mesh" );

		std::string albedo_path = getAssimpMaterialTexturePath( assimp_material, aiTextureType_DIFFUSE, _directory );
		std::string mr_path     = getAssimpMaterialTexturePath( assimp_material, aiTextureType_DIFFUSE_ROUGHNESS, _directory );
		std::string normal_path = getAssimpMaterialTexturePath( assimp_material, aiTextureType_NORMALS, _directory );
		
		mesh->material->addTexture( "uAlbedo",            albedo_path.c_str() );
		mesh->material->addTexture( "uMetallicRoughness", mr_path.c_str() );
		mesh->material->addTexture( "uNormal",            normal_path.c_str() );
	}

	/* create vertex array */
	cm::hVertexArray vertex_array = backend->createVertexArray();
	backend->bindVertexArray( vertex_array );

	/* create vertex buffer */
	cm::sBuffer vertex_buffer = backend->createBuffer( cm::BufferType_Vertex, cm::BufferUsage_Static );
	backend->bufferData( vertex_buffer, vertices.data(), sizeof( sVertex ) * vertices.size() );

	cm::sBuffer index_buffer = backend->createBuffer( cm::BufferType_Index, cm::BufferUsage_Static );
	backend->bufferData( index_buffer, indices.data(), sizeof( unsigned int ) * indices.size() );

	cm::cVertexLayout layout;
	layout.push<float>( 3 ); // pos
	layout.push<float>( 3 ); // normal
	layout.push<float>( 3 ); // tangent
	layout.push<float>( 4 ); // col
	layout.push<float>( 2 ); // texcoord0
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

std::string wv::cContentManager::getAssimpMaterialTexturePath( aiMaterial* _material ,aiTextureType _type, const std::string& _root_dir )
{
	aiString path;
	_material->GetTexture( _type, 0, &path );

	std::string full_path( path.C_Str() );
	full_path = _root_dir + "/" + full_path;
	return full_path;
}

