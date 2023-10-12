#include "Mesh.h"
using namespace WV;

Mesh::Mesh( MeshAsset& _meshAsset ) :
	m_meshAsset( &_meshAsset )
{
	createMeshObject();
}

Mesh::~Mesh()
{
	destroyMeshObject();
}

void Mesh::destroyMeshObject()
{
	delete m_vertexArray;
	m_vertexArray = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;
}

void Mesh::createMeshObject()
{
	m_vertexArray = new VertexArray();
	VertexBuffer* vertexBuffer = new VertexBuffer( m_meshAsset->getVertexData(), m_meshAsset->getVerticeSize() );

	VertexBufferLayout layout;
	layout.Push<float>( 3 ); // pos
	layout.Push<float>( 2 ); // uv
	layout.Push<float>( 3 ); // normal
	m_vertexArray->AddBuffer( *vertexBuffer, layout );

	m_indexBuffer = new IndexBuffer( m_meshAsset->getIndexData(), m_meshAsset->getIndexCount() );
}

void Mesh::recalculateNormals()
{
	m_meshAsset->recalculateNormals();
	createMeshObject();
}
