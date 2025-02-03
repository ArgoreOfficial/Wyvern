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
		return FunctionFlags::kOnLoad | FunctionFlags::kOnUnload | FunctionFlags::kOnDraw;
	}

	virtual void onLoad( void ) override;
	virtual void onUnload( void ) override;

	virtual void onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) override;

protected:

	sMeshInstance m_mesh;
	std::string m_meshPath = "";

};

}