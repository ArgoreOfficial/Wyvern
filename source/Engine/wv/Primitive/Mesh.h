#pragma once

#include <wv/Primitive/Primitive.h>
#include <wv/Types.h>

#include <string>
#include <vector>

namespace wv
{
	struct MeshDesc
	{

	};


	class Mesh
	{
	public:
		std::string name;
		wv::Handle vaoHandle;
		std::vector<Primitive*> primitives;
	};
}