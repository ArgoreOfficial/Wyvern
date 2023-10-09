#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace WV
{
	class VertexArray
	{
	private:
		unsigned int _renderID;
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer( const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout );
		void bind() const;
		void Unbind() const;
	};

}