#pragma once
#include <Wyvern/API/Core.h>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class WYVERN_API VertexArray
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

