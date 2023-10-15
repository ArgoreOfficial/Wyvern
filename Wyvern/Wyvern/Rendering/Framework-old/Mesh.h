#pragma once
#include <Wyvern/Assets/MeshAsset.h>
#include <bgfx/bgfx.h>
// #include "VertexArray.h"
// #include "IndexBuffer.h"

namespace WV
{
	class Mesh
	{
	public:
		Mesh( MeshAsset& _meshAsset );
		~Mesh();
		VertexArray* getVertexArray() { return m_vertexArray; }
		IndexBuffer* getIndexBuffer() { return m_indexBuffer; }
		void recalculateNormals();

	private:
		void destroyMeshObject();
		void createMeshObject();
		MeshAsset* m_meshAsset;

		VertexArray* m_vertexArray;
		IndexBuffer* m_indexBuffer;
	};
}
