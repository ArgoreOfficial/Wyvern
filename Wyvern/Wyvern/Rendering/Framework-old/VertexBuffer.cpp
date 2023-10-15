#include "VertexBuffer.h"
#include <GL/glew.h>
using namespace WV;

VertexBuffer::VertexBuffer()
{ 

}

VertexBuffer::VertexBuffer( void* _data, unsigned int _size ):
	m_size(_size),
	m_vertices(_data)
{
	glGenBuffers( 1, &m_renderID );
	glBindBuffer( GL_ARRAY_BUFFER, m_renderID );
	glBufferData( GL_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW );
}

VertexBuffer::~VertexBuffer()
{ 
	glDeleteBuffers( 1, &m_renderID );
}

void VertexBuffer::bind() const
{
	glBindBuffer( GL_ARRAY_BUFFER, m_renderID );
}

void VertexBuffer::unbind() const
{
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
