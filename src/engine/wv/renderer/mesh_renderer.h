#pragma once

#include <wv/graphics/mesh.h>
#include <wv/math/transform.h>

#include <vector>

namespace wv {

class MeshResource;
class IGraphicsDevice;
struct MeshNode;

class IMeshRenderer
{
public:
	IMeshRenderer( IGraphicsDevice* _pGraphics ) :
		m_pGraphics{ _pGraphics }
	{

	}
	
	virtual void drawMeshNode( MeshNode* _pNode, Matrix4x4f* _pInstanceMatrices, size_t _numInstances );
	virtual void drawMesh( MeshResource* _pMesh );

private:

	IGraphicsDevice* m_pGraphics{ nullptr };
	std::vector<MeshInstanceData> m_instanceDatas{};

};

}