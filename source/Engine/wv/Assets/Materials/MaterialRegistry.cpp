#include "MaterialRegistry.h"

#include <wv/Assets/Materials/Material.h>
#include <wv/Assets/Texture.h>

#include <wv/Shader/ShaderRegistry.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Device/GraphicsDevice.h>
#include <fkYAML/node.hpp>


wv::cMaterial* wv::cMaterialRegistry::loadMaterial( std::string _name )
{
	cMaterial* material = static_cast< cMaterial* >( getLoadedResource( _name ) );

	if( material == nullptr )
	{
		std::string path = "res/materials/" + _name + ".wmat";
		std::string jsonSource = m_pFileSystem->loadString( path );

		material = createMaterialFromSource( _name, jsonSource );

		addResource( material );
	}

	material->incrNumUsers();

	return material;
}

wv::cMaterial* wv::cMaterialRegistry::createMaterialFromSource( std::string _name, std::string _source )
{
	fkyaml::node root = fkyaml::node::deserialize( _source );

	std::string shaderName = root["shader"].get_value<std::string>();
	cShaderProgram* program = m_pShaderRegistry->loadProgramFromWShader( "res/shaders/" + shaderName + ".wshader" );

	while ( !program->isLoaded() ) 
	{
		Sleep( 1 );
	}

	std::vector<sMaterialVariable> variables;
	for( auto& textureObject : root[ "textures" ] )
	{
		std::string uniformName = textureObject[ "name" ].get_value<std::string>();
		std::string textureName = textureObject[ "texture" ].get_value<std::string>();

		if( uniformName == "" || textureName == "" )
		{
			Debug::Print( Debug::WV_PRINT_ERROR, "Texture uniform is empty\n" );
			continue;
		}

		sMaterialVariable textureVariable;
		textureVariable.handle = 0;
		textureVariable.name = uniformName;
		textureVariable.type = WV_MATERIAL_VARIABLE_TEXTURE;
		
		Texture* texture = new Texture( textureName );
		m_resourceLoader.addLoad( texture );
		textureVariable.data.texture = texture;
		
		while ( !texture->isLoaded() ) { }

		variables.push_back( textureVariable );
	}

	cMaterial* mat = new cMaterial( _name, program, variables );
	mat->load( m_pFileSystem );
	mat->create( m_pGraphicsDevice );

	return mat;
}

void wv::cMaterialRegistry::loadBaseMaterials()
{
	m_pShaderRegistry->batchLoadPrograms( 
		{
			"res/shaders/phong.wshader",
			"res/shaders/sky.wshader"
		} );

	loadMaterial( "phong" );
	loadMaterial( "sky" );
}