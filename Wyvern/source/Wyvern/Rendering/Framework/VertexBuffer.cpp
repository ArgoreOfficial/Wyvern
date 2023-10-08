#include "VertexBuffer.h"
#include <GL/glew.h>
using namespace WV;

VertexBuffer::VertexBuffer()
{ }

VertexBuffer::VertexBuffer( const void* data, unsigned int size )
{ 

	glGenBuffers( 1, &_renderID );
	glBindBuffer( GL_ARRAY_BUFFER, _renderID );
	glBufferData( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW );

}

VertexBuffer::~VertexBuffer()
{ 
	glDeleteBuffers( 1, &_renderID );
}

void VertexBuffer::Bind() const
{
	glBindBuffer( GL_ARRAY_BUFFER, _renderID );
}

void VertexBuffer::Unbind() const
{
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
