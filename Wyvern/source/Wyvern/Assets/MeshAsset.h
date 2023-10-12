#pragma once
#include "IAsset.h"
#include <vector>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

namespace WV
{
	class MeshAsset : public IAsset
	{
	public:
		bool load( std::string _path ) override;

		void* getVertexData() { return &( m_vertices[ 0 ] ); }
		unsigned int* getIndexData() { return &( m_indices[ 0 ] ); }

		unsigned int getVerticeSize() { return m_vertices.size() * sizeof( float ); }
		unsigned int getIndexCount() { return m_indices.size(); }
		
		void recalculateNormals();

	private:
		std::vector<float> m_vertices;
		int m_stride;
		std::vector<unsigned int> m_indices;

		inline glm::vec3 calculateTriangleNormal( glm::vec3 _a, glm::vec3 _b, glm::vec3 _c )
		{
			glm::vec3 normal = glm::cross( _c - _a, _b - _a );
			return normal;
		}
	};
}