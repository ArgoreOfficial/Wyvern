#pragma once

#include <wv/Primitive/Primitive.h>
#include <wv/Types.h>
#include <wv/Math/Transform.h>

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
		Transformf transform;
		std::vector<Primitive*> primitives;
	};
}