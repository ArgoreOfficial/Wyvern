#pragma once

#include <lo/types.h>
#include "InputLayout.h"

namespace lo
{
	enum eBufferType
	{
		LO_BUFFER_TYPE_VERTEX,
		LO_BUFFER_TYPE_INDEX
	};

	enum eBufferUsage
	{
		LO_BUFFER_USAGE_STATIC_DRAW,
		LO_BUFFER_USAGE_DYNAMIC_DRAW
	};

	struct sBufferDesc
	{
		eBufferType type;
		eBufferUsage usage;
		sInputLayoutInfo* layout;
		void* data;
		size_t size;
	};

	class cBuffer
	{
	public:
		unsigned int m_handle;
	};
	/*
	template<lo::eBufferType T, lo::eType... Types>
	class cBuffer
	{
	public:
		cBuffer()
		{
			auto list = { ( printf( "%i\n", (int)Types ) )... };
		}

		void buffer( void* _data, size_t _count ) { }

	private:
		int m_stride = 0;

		unsigned int m_handle = 0;
	};
	*/
}
