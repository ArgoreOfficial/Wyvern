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
	
	void add( MeshResource* _pMesh )
	{
		if ( _pMesh == nullptr )
			return;

		for ( auto m : m_drawList )
			if ( _pMesh == m ) // already in list
				return;

		m_drawList.push_back( _pMesh );
	}

	void render();
	
	void flush() 
	{
		m_drawList.clear();
	}

	virtual void drawMeshNode( MeshNode* _pNode, Matrix4x4f* _pInstanceMatrices, size_t _numInstances );
	
private:

	IGraphicsDevice* m_pGraphics{ nullptr };
	std::vector<MeshInstanceData> m_instanceDatas{};

	std::vector<MeshResource*> m_drawList;
};

}