#pragma once

namespace WV
{
	class IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer( unsigned int* _data, unsigned int _count );
		~IndexBuffer();

		void bind() const;
		void unbind() const;

		void glInit();

		const unsigned int GetIndexCount() const { return m_count; }

	private:
		unsigned int m_renderID = 0;
		unsigned int m_count = 0;
		unsigned int* m_indices = nullptr;
	};
}