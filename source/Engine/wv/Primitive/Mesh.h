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
		std::vector<Primitive*> primitives;
	};
}