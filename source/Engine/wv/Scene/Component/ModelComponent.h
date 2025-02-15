#pragma once

#include "IComponent.h"

#include <wv/Mesh/MeshResource.h>

namespace wv
{

class ModelComponent : public IComponent
{
public:
	ModelComponent( const std::string& _meshPath ) :
		IComponent( "ModelComponent" ),
		m_meshPath{ _meshPath }
	{ }

	virtual FunctionFlags getFunctionFlags() override {
		return FunctionFlags::kOnConstruct | FunctionFlags::kOnDestruct | FunctionFlags::kOnDraw;
	}

	virtual void onConstruct( void ) override;
	virtual void onDestruct( void ) override;

	virtual void onDraw( wv::iDeviceContext* _context, wv::IGraphicsDevice* _device ) override;

protected:

	sMeshInstance m_mesh;
	std::string m_meshPath = "";

};

}