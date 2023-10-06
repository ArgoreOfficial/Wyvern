#pragma once
#include <Wyvern/API/Core.h>

class WYVERN_API IndexBuffer
{
private:
	unsigned int _renderID;
	unsigned int _count;
public:
	IndexBuffer();
	IndexBuffer( const unsigned int* data, unsigned int count );
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	const unsigned int GetIndexCount() const { return _count; }
};