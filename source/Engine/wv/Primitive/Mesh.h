#pragma once

#include <wv/Primitive/Primitive.h>
#include <wv/Types.h>
#include <wv/Assets/Materials/IMaterial.h>

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