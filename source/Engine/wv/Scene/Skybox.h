#pragma once

#include "SceneObject.h"

#include <string>
#include <vector>

#include <wv/Reflection/Reflection.h>
#include <wv/Mesh/MeshResource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode;
	class cMaterial;

///////////////////////////////////////////////////////////////////////////////////////

	class cSkyboxObject : public iSceneObject
	{

	public:

		 cSkyboxObject( const UUID& _uuid, const std::string& _name );
		~cSkyboxObject();

///////////////////////////////////////////////////////////////////////////////////////

		static cSkyboxObject* parseInstance( sParseData& _data );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void onLoadImpl   () override;
		void onUnloadImpl () override;
		void onCreateImpl () override;
		void onDestroyImpl() override;

		virtual void updateImpl( double _deltaTime ) override;
		virtual void drawImpl  ( iDeviceContext* _context, iGraphicsDevice* _device ) override;

		sMeshInstance m_mesh;

	};
}