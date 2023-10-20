#pragma once
#include <vector>
#include <string>
#include <Wyvern/Assets/Asset.h>

namespace WV
{
	class Mesh
	{
	public:
		struct Vertex
		{
			/*
			float x;
			float y;
			float z;
			float normal_x;
			float normal_y;
			float normal_z;
			float u;
			float v;
			*/
			float x;
			float y;
			float z;
			uint32_t abgr;
		};

		void loadOBJ( std::string _path );
		std::vector<Vertex>& getVertices() { return m_vertices; };
		std::vector<uint16_t>& getIndices() { return m_indices; }

	private:
		std::vector<Vertex> m_vertices;
		std::vector<uint16_t> m_indices;
	};
}