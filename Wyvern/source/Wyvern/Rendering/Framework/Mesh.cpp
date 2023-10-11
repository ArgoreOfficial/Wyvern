#include "Mesh.h"
using namespace WV;

Mesh::Mesh( MeshAsset& _meshAsset )
{
	m_vertexArray = new VertexArray();
	VertexBuffer* vertexBuffer = new VertexBuffer( _meshAsset.getVertexData(), _meshAsset.getVerticeSize());

	VertexBufferLayout layout;
	layout.Push<float>( 3 ); // pos
	layout.Push<float>( 2 ); // uv
	m_vertexArray->AddBuffer( *vertexBuffer, layout );

	m_indexBuffer = new IndexBuffer( _meshAsset.getIndexData(), _meshAsset.getIndexCount());
}

Mesh::~Mesh()
{

}
