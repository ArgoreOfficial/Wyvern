#pragma once

#include <wv/types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

	struct VertexAttribute
	{
		const char* name;
		unsigned int componentCount;
		DataType type;
		bool normalized;
		unsigned int size;
	};

	///////////////////////////////////////////////////////////////////////////////////////

	struct VertexLayout
	{
		VertexAttribute* elements = nullptr;
		unsigned int numElements = 0;
	};

}

