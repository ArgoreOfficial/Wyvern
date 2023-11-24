#pragma once

namespace wv
{
	class cIndexBuffer
	{
	public:
		 cIndexBuffer();
		~cIndexBuffer();

		void create();
		void bufferData( unsigned int* _data, unsigned long long _size );
		void bind();

	private:
		unsigned int m_handle = 0;
	};
}