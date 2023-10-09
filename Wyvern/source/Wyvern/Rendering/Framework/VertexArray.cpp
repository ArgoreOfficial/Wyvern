#include "VertexArray.h"
using namespace WV;

VertexArray::VertexArray()
{ 
	glGenVertexArrays( 1, &_renderID );
}

VertexArray::~VertexArray()
{ 
	glDeleteVertexArrays( 1, &_renderID );
}

void VertexArray::AddBuffer( const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout )
{ 
	bind();
	vertexBuffer.bind();
	
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for ( unsigned int i = 0; i < elements.size(); i++ )
	{
		const auto& element = elements[ i ];
		glEnableVertexAttribArray( i );
		glVertexAttribPointer( i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::GetSizeOfType( element.type );
	}

}

void VertexArray::bind() const
{ 
	glBindVertexArray( _renderID );
}

void VertexArray::Unbind() const
{
	glBindVertexArray( 0 );
}
