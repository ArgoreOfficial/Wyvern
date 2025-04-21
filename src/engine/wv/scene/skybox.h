#pragma once

#include "entity/entity.h"

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

	class SkyboxObject : public Entity
	{

	public:

		 SkyboxObject( const UUID& _uuid, const std::string& _name );
		~SkyboxObject();

///////////////////////////////////////////////////////////////////////////////////////

		static SkyboxObject* parseInstance( ParseData& _data );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		virtual void onConstruct( void ) override;
		virtual void onDestruct ( void ) override;
		
		virtual void onUpdate( double _deltaTime ) override;
		virtual void onDraw  ( IDeviceContext* _context, IGraphicsDevice* _device ) override;

		MeshInstance m_mesh;

	};
}