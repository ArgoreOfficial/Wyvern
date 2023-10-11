#pragma once
#include "IAsset.h"
#include <vector>

namespace WV
{
	class MeshAsset : public IAsset
	{
	public:
		bool load( std::string _path, std::string _name = "" ) override;

		void* getVertexData() { return &( m_vertices[ 0 ] ); }
		unsigned int* getIndexData() { return &( m_indices[ 0 ] ); }

		unsigned int getVerticeSize() { return m_vertices.size() * sizeof( float ); }
		unsigned int getIndexCount() { return m_indices.size(); }
	
	private:
		std::vector<float> m_vertices;
		std::vector<unsigned int> m_indices;
	};
}