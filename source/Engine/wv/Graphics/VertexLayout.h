#pragma once

#include <wv/Types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

	struct sVertexAttribute
	{
		const char* name;
		unsigned int componentCount;
		DataType type;
		bool normalized;
		unsigned int size;
	};

	///////////////////////////////////////////////////////////////////////////////////////

	struct sVertexLayout
	{
		sVertexAttribute* elements;
		unsigned int numElements;
	};

}

