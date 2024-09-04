#include "MaterialRegistry.h"

#include <wv/Assets/Materials/Material.h>
#include <wv/Assets/Texture.h>

#include <wv/Shader/ShaderRegistry.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Auxiliary/json/json11.hpp>

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
	std::string err;
	json11::Json root = json11::Json::parse( _source, err );

	std::string shaderName = root["shader"].string_value();
	cShaderProgram* program = m_pShaderRegistry->loadProgramFromWShader( "res/shaders/" + shaderName + ".wshader" );

	while ( !program->isLoaded() ) 
	{
		//Sleep( 1 );
	}

	std::vector<sMaterialVariable> variables;
	for( auto& textureObject : root[ "textures" ].array_items() )
	{
		std::string uniformName = textureObject[ "name" ].string_value();
		std::string textureName = textureObject[ "texture" ].string_value();

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