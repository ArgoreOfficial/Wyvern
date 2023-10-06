#pragma once
#include <Wyvern/API/Core.h>

class WYVERN_API VertexBuffer
{
private:
	unsigned int _renderID;
public:
	VertexBuffer();
	VertexBuffer( const void* _data, unsigned int _size );
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

};