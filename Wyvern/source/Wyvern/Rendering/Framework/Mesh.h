#pragma once
#include <Wyvern/Assets/MeshAsset.h>
#include "VertexArray.h"
#include "IndexBuffer.h"

namespace WV
{
	class Mesh
	{
	public:
		Mesh( MeshAsset& _meshAsset );
		~Mesh();

		VertexArray* getVertexArray() { return m_vertexArray; }
		IndexBuffer* getIndexBuffer() { return m_indexBuffer; }

	private:
		VertexArray* m_vertexArray;
		IndexBuffer* m_indexBuffer;
	};
}