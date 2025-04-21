#pragma once

#include "entity/entity.h"

#include <wv/mesh/mesh_resource.h>
#include <wv/runtime.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

///////////////////////////////////////////////////////////////////////////////////////

WV_RUNTIME_OBJECT( ModelObject, Entity )
class ModelObject : public Entity
{
public:
	ModelObject() = default;
	ModelObject( const UUID& _uuid, const std::string& _name );
	~ModelObject() { }
	/*
	static ModelObject* parseInstance( ParseData& _data ) {
		return Runtime::create<ModelObject>();
	}
	*/

	virtual FunctionFlags getFunctionFlags() override {
		return FunctionFlags::kOnConstruct | FunctionFlags::kOnDestruct | FunctionFlags::kOnDraw;
	}

	static void queryProperties( wv::RuntimeProperties* _pOutProps ) {
		_pOutProps->add( "path", &ModelObject::m_meshPath );
	}

	virtual void onConstruct( void ) override;
	virtual void onDestruct( void ) override;

	virtual void onDraw( wv::IDeviceContext* _context, wv::IGraphicsDevice* _device );

///////////////////////////////////////////////////////////////////////////////////////

protected:
	MeshInstance m_mesh;
	std::string m_meshPath = "";

};
}