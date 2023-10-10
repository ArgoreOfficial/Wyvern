#pragma once

namespace WV
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		VertexBuffer( void* _data, unsigned int _size );
		~VertexBuffer();

		void bind() const;
		void unbind() const;

		void glInit();

	private:
		unsigned int m_renderID = 0;
		unsigned int m_size = 0;
		void* m_vertices = nullptr;
	};
}