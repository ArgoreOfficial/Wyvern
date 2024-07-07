#include "MaterialRegistry.h"

#include <wv/Assets/Materials/Material.h>
#include <wv/Shader/ShaderRegistry.h>
#include <wv/Memory/MemoryDevice.h>

#include <wv/Assets/Materials/DefaultLitMaterial.h>

void wv::cMaterialRegistry::loadBaseMaterials()
{
	m_pShaderRegistry->batchLoadPrograms( 
		{
			"phong"
		} );

	addResource( new cPhongMaterial( m_pShaderRegistry->loadProgramFromWShader( "phong" ) ) );
}