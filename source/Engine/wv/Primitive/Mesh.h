#pragma once

#include <wv/Primitive/Primitive.h>
#include <wv/Types.h>

#include <vector>

namespace wv
{
	struct MeshDesc
	{

	};


	class Mesh
	{
	public:
		wv::Handle vaoHandle;
		std::vector<Primitive*> primitives;
	};
}