#pragma once
#include <vector>
#include <string>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>
namespace wv
{
	namespace gltf
	{
		struct Scene
		{
			std::string name;
			std::vector<int> nodes;
		};

		struct Node
		{
			int mesh = -1;
			std::string name;
			wv::Vector4f rotation;
			wv::Vector3f translation;
		};

		struct Primitive
		{
			std::vector<std::pair<std::string, int>> attributes;
			int indices;
		};

		struct Mesh
		{
			std::string name;
			std::vector<gltf::Primitive> primitives;
		};

		struct Accessor
		{
			int bufferView;
			int componentType;
			int count;
			union {
				std::vector<char> b;
				std::vector<unsigned char> ub;
				std::vector<short> s;
				std::vector<unsigned short> us;
				std::vector<unsigned int> ui;
				std::vector<float> f;
			} max;
			union
			{
				std::vector<char> b;
				std::vector<unsigned char> ub;
				std::vector<short> s;
				std::vector<unsigned short> us;
				std::vector<unsigned int> ui;
				std::vector<float> f;
			} min;
			std::string type;
		};

		class Parser
		{
		public:
			Parser();
		};
	}
}