#pragma once

#include <wv/Math/Transform.h>
#include <wv/Math/Triangle.h>
#include <wv/Primitive/Primitive.h>
#include <wv/Types.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct MeshDesc
	{

	};

///////////////////////////////////////////////////////////////////////////////////////

	class sMesh
	{

	public:

		std::string name;
		Transformf transform;
		std::vector<Primitive*> primitives;
		std::vector<Triangle3f> triangles;
	};

	struct sMeshNode
	{
		std::string name;
		Transformf transform;
		std::vector<sMesh*>      meshes;
		std::vector<sMeshNode*> children;
	};
}