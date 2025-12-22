#pragma once

#include <wv/entity/entity_component.h>
#include <wv/types.h>

namespace wv {

class MeshComponent : public IEntityComponent
{
	WV_REFLECT_TYPE( MeshComponent )
public:
	MeshComponent() = default;
	virtual ~MeshComponent();

	void setRenderMesh( ResourceID _renderMesh ) { m_renderMesh = _renderMesh; }
	ResourceID getRenderMesh() const { return m_renderMesh; }

private:

	ResourceID m_renderMesh;
};

}