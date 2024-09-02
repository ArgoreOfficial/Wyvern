#include "ShaderRegistry.h"

#include <wv/Memory/MemoryDevice.h>

#include <wv/Debug/Print.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Shader/ShaderProgram.h>

#include <wv/Resource/cResourceLoader.h>

wv::cShader* wv::cShaderRegistry::loadShader( eShaderType _type, const std::string& _name )
{
	cShader* shader = static_cast<cShader*>( getLoadedResource( _name ) );

	if ( shader == nullptr )
	{
		shader = new cShader( _type, _name );
		m_resourceLoader.addLoad( shader );
		//shader->load( m_pFileSystem );

		addResource( shader );
	}

	shader->incrNumUsers();

    return shader;
}

wv::cShaderProgram* wv::cShaderRegistry::loadProgramFromWShader( const std::string& _name )
{
	cShaderProgram* program = static_cast<cShaderProgram*>( getLoadedResource( _name ) );
	if ( program )
	{
		program->incrNumUsers();
		return program;
	}

	program = new cShaderProgram( this, _name );
	m_resourceLoader.addLoad( program );
	addResource( program );
	
	return program;
}

void wv::cShaderRegistry::batchLoadPrograms( const std::vector<std::string>& _names )
{
	if ( _names.empty() )
		return;

	for ( size_t i = 0; i < _names.size(); i++ )
	{
		cShaderProgram* program = static_cast<cShaderProgram*>( getLoadedResource( _names[ i ] ) );
		if ( program ) // program has already been loaded, just continue
			continue;
		
		program = new cShaderProgram( this, _names[ i ] );

		m_resourceLoader.addLoad( program );
		addResource( program );
	}
}

void wv::cShaderRegistry::unloadShaderProgram( cShaderProgram* _program )
{
	std::vector<cShader*> shaders = _program->getShaders();
	findAndUnloadResource( _program );

	for ( int i = 0; i < shaders.size(); i++ )
		findAndUnloadResource( shaders[ i ] );
}
