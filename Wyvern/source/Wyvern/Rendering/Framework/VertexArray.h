#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace WV
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void glInit();

		void AddBuffer( VertexBuffer* vertexBuffer, VertexBufferLayout layout );
		void bind() const;
		void Unbind() const;
	private:
		unsigned int _renderID;

		VertexBuffer* m_vertexBuffer;
		VertexBufferLayout m_layout;
	};

}