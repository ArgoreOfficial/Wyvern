#pragma once

#include "scene_object.h"

#include <string>
#include <vector>

#include <wv/reflection/reflection.h>
#include <wv/mesh/mesh_resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode;
	class cMaterial;

///////////////////////////////////////////////////////////////////////////////////////

	class cSkyboxObject : public IEntity
	{

	public:

		 cSkyboxObject( const UUID& _uuid, const std::string& _name );
		~cSkyboxObject();

///////////////////////////////////////////////////////////////////////////////////////

		static cSkyboxObject* parseInstance( sParseData& _data );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void onConstructImpl   () override;
		void onDeconstructImpl () override;
		void onEnterImpl () override;
		void onExitImpl() override;

		virtual void onUpdate( double _deltaTime ) override;
		virtual void onDraw  ( iDeviceContext* _context, IGraphicsDevice* _device ) override;

		sMeshInstance m_mesh;

	};
}