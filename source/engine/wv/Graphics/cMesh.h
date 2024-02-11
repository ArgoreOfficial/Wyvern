#pragma once

#include <cm/Framework/VertexArray.h>
#include <cm/Framework/Buffer.h>
#include <cm/Framework/cVertexLayout.h>

namespace wv
{
	class cMesh
	{
	public:
		 cMesh() { }
		~cMesh() { }

		cm::hVertexArray vertex_array;

		cm::sBuffer vertex_buffer;
		cm::sBuffer index_buffer;

		cm::cVertexLayout layout;

	private:

	};
}
