#pragma once

#include "SceneObject.h"

#include <wv/Reflection/Reflection.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Mesh;
	struct sMeshNode;

	class cMaterial;

///////////////////////////////////////////////////////////////////////////////////////

	class cModelObject : public iSceneObject
	{

	public:

		 cModelObject( const UUID& _uuid, const std::string& _name, sMeshNode* _meshNode );
		 cModelObject( const UUID& _uuid, const std::string& _name, const std::string& _meshPath );
		~cModelObject();
		

		static cModelObject* parseInstance( sParseData& _data );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void onLoadImpl() override;
		void onUnloadImpl() override;
		void onCreateImpl() override { };
		void onDestroyImpl() override { };

		virtual void updateImpl( double _deltaTime ) override;
		virtual void drawImpl  ( iDeviceContext* _context, iGraphicsDevice* _device ) override;

		sMeshNode* m_mesh = nullptr;
		std::string m_meshPath = "";
	};
}