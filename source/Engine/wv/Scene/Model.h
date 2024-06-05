#pragma once

#include "Node.h"

#include <string>
#include <vector>

namespace wv
{
	class Mesh;
	class Pipeline;
	class IMaterial;
	
	class Model : public Node
	{
	public:
		 Model( const uint64_t& _uuid, const std::string& _name );
		~Model();

		void loadFromFile( const std::string& _path );
		virtual void update( double _deltaTime ) override;
		virtual void draw  ( Context* _context, GraphicsDevice* _device ) override;

		IMaterial* m_material; /// TODO: change to index?
	private:
		Mesh* m_mesh; /// TODO: allow for multiple meshes
	};
}