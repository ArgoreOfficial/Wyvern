#pragma once

#include <cm/Framework/VertexArray.h>
#include <cm/Framework/Buffer.h>
#include <cm/Framework/cVertexLayout.h>

#include <wv/Graphics/cMaterial.h>

#include <wv/Math/Transform.h>
#include <wv/Math/Vector2.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

namespace wv
{
	struct sVertex
	{
		wv::cVector3f position;
		wv::cVector3f normal;
		wv::cVector3f tangent;
		wv::cVector4f color;
		wv::cVector2f tex_coord_0;
	};

	class cMesh
	{
	public:
		 cMesh() { }
		~cMesh();

		cm::hVertexArray vertex_array{};

		cm::sBuffer vertex_buffer{};
		cm::sBuffer index_buffer {};

		int num_vertices = 0;
		int num_indices  = 0;

		cm::cVertexLayout layout{};

		cMaterial* material = nullptr;

		cTransformf transform;

	private:

	};
}
