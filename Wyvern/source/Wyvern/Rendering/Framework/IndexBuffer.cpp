#include "IndexBuffer.h"
#include <GL/glew.h>
using namespace WV;

IndexBuffer::IndexBuffer()
{

}

IndexBuffer::IndexBuffer( unsigned int* _data, unsigned int _count ) :
	m_count( _count ),
	m_indices( _data )
{
	glGenBuffers( 1, &m_renderID );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_renderID );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, _count * sizeof( unsigned int ), _data, GL_STATIC_DRAW );
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers( 1, &m_renderID );
}

void IndexBuffer::bind() const
{
	glBindBuffer( GL_ARRAY_BUFFER, m_renderID );
}

void IndexBuffer::unbind() const
{
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
