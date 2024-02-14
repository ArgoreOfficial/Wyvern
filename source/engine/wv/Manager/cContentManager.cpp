#include "cContentManager.h"

#include <cm/Core/stb_image.h>
#include <cm/Backends/iBackend.h>

#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>

#include <wv/Graphics/cMaterial.h>
#include <wv/Graphics/cShader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

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
