#pragma once

#include <wv/Resource/ResourceRegistry.h>

namespace wv
{
	class iGraphicsDevice;
	class cMaterial;

	class cMaterialRegistry : public iResourceRegistry
	{
	public:
		cMaterialRegistry( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) :
			iResourceRegistry{ "MaterialRegistry", _pFileSystem, _pGraphicsDevice },
			m_pGraphicsDevice{ _pGraphicsDevice }
		{
			// loadBaseMaterials();
		}

		cMaterial* loadMaterial( const std::string& _name );

	private:

		cMaterial* createMaterialFromSource( std::string _name, std::string _source );

		void loadBaseMaterials();

		iGraphicsDevice* m_pGraphicsDevice;

		std::unordered_map<std::string, cMaterial*> m_materials;

	};
}