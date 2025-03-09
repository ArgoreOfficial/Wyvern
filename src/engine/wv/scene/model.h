#pragma once

#include "entity/entity.h"

#include <wv/mesh/mesh_resource.h>
#include <wv/reflection/reflection.h>
#include <wv/runtime.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

///////////////////////////////////////////////////////////////////////////////////////

WV_RUNTIME_OBJECT( ModelObject, IRuntimeObject )
class ModelObject : public Entity, public IRuntimeObject
{
public:
	ModelObject() = default;
	ModelObject( const UUID& _uuid, const std::string& _name );
	~ModelObject() { }

	static ModelObject* parseInstance( ParseData& _data ) {
		return Runtime::create<ModelObject>();
	}

	virtual FunctionFlags getFunctionFlags() override {
		return FunctionFlags::kOnConstruct | FunctionFlags::kOnDestruct;
	}

	static void queryProperties( wv::RuntimeProperties* _pOutProps ) {
		_pOutProps->add( "path", &ModelObject::m_meshPath );
	}

	virtual void onConstruct( void ) override;
	virtual void onDestruct( void ) override;

///////////////////////////////////////////////////////////////////////////////////////

protected:
	MeshInstance m_mesh;
	std::string m_meshPath = "";

};
}