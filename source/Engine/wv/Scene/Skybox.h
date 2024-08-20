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

	class cSkyboxObject : public iSceneObject
	{

	public:

		 cSkyboxObject( const uint64_t& _uuid, const std::string& _name );
		~cSkyboxObject();

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void onLoadImpl   () override;
		void onUnloadImpl () override;
		void onCreateImpl () override;
		void onDestroyImpl() override;

		virtual void updateImpl( double _deltaTime ) override;
		virtual void drawImpl  ( iDeviceContext* _context, iGraphicsDevice* _device ) override;

		Mesh*      m_skyboxMesh  = nullptr;
		cMaterial* m_skyMaterial = nullptr;

	};

}