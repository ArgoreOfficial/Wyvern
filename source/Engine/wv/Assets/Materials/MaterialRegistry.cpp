#include "MaterialRegistry.h"

#include <wv/Assets/Materials/Material.h>
#include <wv/Assets/Texture.h>

#include <wv/Memory/FileSystem.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Auxiliary/json/json11.hpp>

wv::cMaterial* wv::cMaterialRegistry::loadMaterial( const std::string& _name )
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

	cProgramPipeline* pipeline = new cProgramPipeline( shaderName );
	pipeline->load( m_pFileSystem, m_pGraphicsDevice );

	while ( !pipeline->isComplete() ) 
	{
		/// TEMPORARY FIX
		/// TODO: NOT THIS
	#ifdef WV_PLATFORM_WINDOWS 
		Sleep( 1 );
	#endif
	}
	
	std::vector<sMaterialVariable> variables;

#ifdef WV_PLATFORM_WINDOWS
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
		
		while ( !texture->isComplete() ) 
		{
		#ifdef WV_PLATFORM_WINDOWS 
			Sleep( 1 );
			m_pGraphicsDevice->endRender();
		#endif
		}

		variables.push_back( textureVariable );
	}

#endif

	cMaterial* mat = new cMaterial( _name, pipeline, variables );
	mat->load( m_pFileSystem, m_pGraphicsDevice );
	
	return mat;
}

void wv::cMaterialRegistry::loadBaseMaterials()
{
	loadMaterial( "DefaultMaterial" );
	loadMaterial( "DebugTextureMaterial" );
	loadMaterial( "SkyMaterial" );
}