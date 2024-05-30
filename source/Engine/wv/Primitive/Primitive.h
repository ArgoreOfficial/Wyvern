#pragma once

#include <wv/Types.h>

namespace wv
{

	struct InputLayoutElement
	{
		unsigned int num;
		DataType type;
		bool normalized;
		unsigned int size;
	};

	struct InputLayout
	{
		InputLayoutElement* elements;
		unsigned int numElements;
	};
	
	////////////////////////////////////////////

	enum PrimitiveType
	{
		WV_PRIMITIVE_TYPE_STATIC
	};

	struct PrimitiveDesc
	{
		PrimitiveType type;
		InputLayout* layout;

		void* vertexBuffer;
		unsigned int vertexBufferSize;
		unsigned int numVertices;

		void* indexBuffer;
		unsigned int indexBufferSize;
		unsigned int numIndices;
	};

	class Primitive
	{
	public:
		wv::Handle handle;
		PrimitiveType type;
		uint32_t count;
	};
}