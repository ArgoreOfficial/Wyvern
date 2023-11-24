#include <Wyvern/Renderer/Framework/cIndexBuffer.h>
#include <glad/gl.h>

using namespace wv;

cIndexBuffer::cIndexBuffer()
{

}

cIndexBuffer::~cIndexBuffer()
{

}

void wv::cIndexBuffer::create()
{
	glGenBuffers( 1, &m_handle );
	bind();
}

void wv::cIndexBuffer::bufferData( unsigned int* _data, unsigned long long _size )
{
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW );
}

void wv::cIndexBuffer::bind()
{
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_handle );
}
