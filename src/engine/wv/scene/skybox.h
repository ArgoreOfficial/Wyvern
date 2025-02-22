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

	struct MeshNode;
	class Material;

///////////////////////////////////////////////////////////////////////////////////////

	class SkyboxObject : public IEntity
	{

	public:

		 SkyboxObject( const UUID& _uuid, const std::string& _name );
		~SkyboxObject();

///////////////////////////////////////////////////////////////////////////////////////

		static SkyboxObject* parseInstance( ParseData& _data );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void onConstructImpl   () override;
		void onDeconstructImpl () override;
		void onEnterImpl () override;
		void onExitImpl() override;

		virtual void onUpdate( double _deltaTime ) override;
		virtual void onDraw  ( IDeviceContext* _context, IGraphicsDevice* _device ) override;

		MeshInstance* m_mesh;

	};
}