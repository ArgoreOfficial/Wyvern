#include "cMesh.h"

#include <Wyvern/Filesystem/cFilesystem.h>
#include <Wyvern/Managers/cAssetManager.h>

#include <glad/gl.h>

using namespace wv;

wv::cMesh::cMesh( void )
{
	
}

wv::cMesh::~cMesh( void )
{
	
}

void cMesh::create( void )
{

	m_vertexArray.create();

	m_indexBuffer.create();
	m_indexBuffer.bufferData( indices.data(), indices.size() * sizeof(unsigned int));

	m_vertexBuffer.create();
	m_vertexBuffer.bufferData( vertices.data(), vertices.size() * sizeof( float ));

	cVertexBufferLayout layout;
	layout.push( WV_TYPE::WV_FLOAT, 3 );
	// layout.push( WV_TYPE::WV_FLOAT, 3 );

	m_vertexArray.addLayout( layout );
}

void wv::cMesh::render()
{

	m_vertexArray.bind();
	m_indexBuffer.bind();
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

}
