#pragma once
#include <vector>
#include "GLHeader.h"

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType( unsigned int type )
	{
		switch ( type )
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		}
		return 0;
	}
};

class VertexBufferLayout
{ 
private:
	std::vector<VertexBufferElement> _elements;
	unsigned int _stride;
public:
	VertexBufferLayout() :
		_stride(0)
	{ }
	~VertexBufferLayout() { }

	template<typename T>
	void Push( unsigned int count )
	{ }

	template<>
	void Push<float>( unsigned int count )
	{
		_elements.push_back( { GL_FLOAT, count, GL_FALSE } );
		_stride += VertexBufferElement::GetSizeOfType( GL_FLOAT ) * count;
	}
	template<>
	void Push<unsigned int>( unsigned int count )
	{
		_elements.push_back( { GL_UNSIGNED_INT, count, GL_FALSE } );
		_stride += VertexBufferElement::GetSizeOfType( GL_UNSIGNED_INT ) * count;
	}
	template<>
	void Push<unsigned char>( unsigned int count )
	{
		_elements.push_back( { GL_UNSIGNED_BYTE, count, GL_TRUE } );
		_stride += VertexBufferElement::GetSizeOfType( GL_UNSIGNED_BYTE ) * count;
	}

	const std::vector<VertexBufferElement> GetElements() const { return _elements; }
	unsigned int GetStride() const { return _stride; }
	
};

