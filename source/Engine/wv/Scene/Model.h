#pragma once

#include "SceneObject.h"

#include <wv/Reflection/Reflection.h>
#include <wv/Mesh/MeshResource.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cMeshResource;

	class cMaterial;

///////////////////////////////////////////////////////////////////////////////////////

	class cModelObject : public IEntity
	{

	public:

		 cModelObject( const UUID& _uuid, const std::string& _name, const std::string& _meshPath );
		~cModelObject();
		

		static cModelObject* parseInstance( sParseData& _data );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void onLoadImpl() override;
		void onUnloadImpl() override;
		void onCreateImpl() override { };
		void onDestroyImpl() override { };

		virtual void onUpdate( double _deltaTime ) override;
		virtual void drawImpl  ( iDeviceContext* _context, iLowLevelGraphics* _device ) override;

		sMeshInstance m_mesh;
		std::string m_meshPath = "";
	};
}