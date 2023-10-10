#include "VertexArray.h"
using namespace WV;

VertexArray::VertexArray()
{ 
	
}

VertexArray::~VertexArray()
{ 
	glDeleteVertexArrays( 1, &_renderID );
}

void WV::VertexArray::glInit()
{
	glGenVertexArrays( 1, &_renderID );

	m_vertexBuffer->glInit();

	bind();
	m_vertexBuffer->bind();

	const auto& elements = m_layout.GetElements();
	unsigned int offset = 0;
	for ( unsigned int i = 0; i < elements.size(); i++ )
	{
		const auto& element = elements[ i ];
		glEnableVertexAttribArray( i );
		glVertexAttribPointer( i, element.count, element.type, element.normalized, m_layout.GetStride(), (const void*)offset );
		offset += element.count * VertexBufferElement::GetSizeOfType( element.type );
	}
}

void VertexArray::AddBuffer( VertexBuffer* _vertexBuffer, VertexBufferLayout _layout )
{ 
	m_vertexBuffer = _vertexBuffer;
	m_layout = _layout;
}

void VertexArray::bind() const
{ 
	glBindVertexArray( _renderID );
}

void VertexArray::Unbind() const
{
	glBindVertexArray( 0 );
}
