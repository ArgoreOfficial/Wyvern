#pragma once

#include <wv/Resource/ResourceRegistry.h>

namespace wv
{
	class iGraphicsDevice;
	class cShaderRegistry;
	class cMaterial;

	class cMaterialRegistry : public iResourceRegistry
	{
	public:
		cMaterialRegistry( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice, cShaderRegistry* m_pShaderRegistry ) :
			iResourceRegistry{ "MaterialRegistry", _pFileSystem, _pGraphicsDevice },
			m_pGraphicsDevice{ _pGraphicsDevice },
			m_pShaderRegistry{ m_pShaderRegistry }
		{
			
		}

		cMaterial* loadMaterial( std::string _name );

	private:

		void loadBaseMaterials();

		iGraphicsDevice* m_pGraphicsDevice;
		cShaderRegistry* m_pShaderRegistry;

		std::unordered_map<std::string, cMaterial*> m_materials;

	};
}