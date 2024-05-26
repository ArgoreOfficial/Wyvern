#pragma once

#include <wv/Types.h>

namespace wv
{

	struct InputLayoutElement
	{
		size_t   num;
		DataType type;
		bool     normalized;
		size_t   stride;
	};

	struct InputLayout
	{
		InputLayoutElement* elements;
		size_t numElements;
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
		size_t vertexBufferSize;
		size_t numVertices;

		void* indexBuffer;
		size_t indexBufferSize;
		size_t numIndices;
	};

	class Primitive
	{
	public:
		wv::Handle handle;
		PrimitiveType type;
		uint32_t count;
	};
}