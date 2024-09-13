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

	class Mesh
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
		std::vector<Mesh*>      meshes;
		std::vector<sMeshNode*> children;

		void update()
		{
			transform.update();

			for( auto& mesh : meshes )
				mesh->transform.update();
			
			for( auto& child : children )
				child->update();
		}
	};
}