#pragma once

#include "SceneObject.h"

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Mesh;
	class cMaterial;

///////////////////////////////////////////////////////////////////////////////////////

	class cModelObject : public iSceneObject
	{

	public:

		 cModelObject( const UUID& _uuid, const std::string& _name, Mesh* _mesh );
		~cModelObject();

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void onLoadImpl() override;
		void onUnloadImpl() override;
		void onCreateImpl() override { };
		void onDestroyImpl() override { };

		virtual void updateImpl( double _deltaTime ) override;
		virtual void drawImpl  ( iDeviceContext* _context, iGraphicsDevice* _device ) override;

		Mesh* m_mesh;

	};

}