#include "IndexBuffer.h"
#include <GL/glew.h>

IndexBuffer::IndexBuffer()
{ }

IndexBuffer::IndexBuffer( const unsigned int* data, unsigned int count ):
	_count(count)
{ 
	glGenBuffers( 1, &_renderID );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _renderID );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW );
}

IndexBuffer::~IndexBuffer()
{ 
	glDeleteBuffers( 1, &_renderID );
}

void IndexBuffer::Bind() const
{
	glBindBuffer( GL_ARRAY_BUFFER, _renderID );
}

void IndexBuffer::Unbind() const
{
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
