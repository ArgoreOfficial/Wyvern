#pragma once

#include "../../types.h"
#include "InputLayout.h"

namespace lo
{
	enum eBufferType
	{
		BufferType_Vertex,
		BufferType_Index
	};

	struct sBufferInfo
	{
		eBufferType type;

	};

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
}
