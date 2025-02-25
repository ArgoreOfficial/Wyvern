#pragma once

#include "icomponent.h"

#include <wv/mesh/mesh_resource.h>

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
		return FunctionFlags::kOnConstruct | FunctionFlags::kOnDestruct;
	}

	virtual void onConstruct( void ) override;
	virtual void onDestruct( void ) override;

protected:

	MeshInstance m_mesh;
	std::string m_meshPath = "";

};

}