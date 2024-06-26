#pragma once

#include "Node.h"

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Mesh;
	class Material;

///////////////////////////////////////////////////////////////////////////////////////

	class Model : public Node
	{

	public:

		 Model( const uint64_t& _uuid, const std::string& _name );
		~Model();

		void loadMemory( const std::string& _path );
		virtual void update( double _deltaTime ) override;
		virtual void draw  ( iDeviceContext* _context, iGraphicsDevice* _device ) override;

		Material* m_material; /// TODO: change to index?

///////////////////////////////////////////////////////////////////////////////////////

	private:

		Mesh* m_mesh; /// TODO: allow for multiple meshes

	};

}