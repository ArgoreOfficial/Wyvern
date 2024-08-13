#include "MaterialRegistry.h"

#include <wv/Assets/Materials/Material.h>
#include <wv/Shader/ShaderRegistry.h>
#include <wv/Memory/MemoryDevice.h>

wv::cMaterial* wv::cMaterialRegistry::loadMaterial( std::string _name )
{
	return nullptr;
}

void wv::cMaterialRegistry::loadBaseMaterials()
{
	m_pShaderRegistry->batchLoadPrograms( 
		{
			"phong"
		} );

	// addResource( new cPhongMaterial( m_pShaderRegistry->loadProgramFromWShader( "phong" ) ) );
}