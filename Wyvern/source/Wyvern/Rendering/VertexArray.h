#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{ 
private:
	unsigned int _renderID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer( const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout );
	void Bind() const;
	void Unbind() const;
};

