#pragma once

#include "entity/entity.h"

#include <string>
#include <vector>

#include <wv/mesh/mesh_resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

WV_RUNTIME_OBJECT( SkyboxObject, wv::Entity )
class SkyboxObject : public Entity
{

public:

	 SkyboxObject( void ) = default;
	 SkyboxObject( const UUID& _uuid, const std::string& _name );
	~SkyboxObject();

///////////////////////////////////////////////////////////////////////////////////////

protected:

	virtual void onConstruct( void ) override;
	virtual void onDestruct ( void ) override;
		
	virtual void onUpdate( double _deltaTime ) override;
	virtual void onDraw  ( IDeviceContext* _context, IGraphicsDevice* _device ) override;

	MeshInstance m_mesh;

};

}