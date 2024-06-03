#pragma once

#include "Node.h"
#include <string>

namespace wv
{
	class Primitive;
	class Pipeline;
	class IMaterial;
	
	class Mesh : public Node
	{
	public:
		 Mesh( const uint64_t& _uuid, const std::string& _name );
		~Mesh();

		void loadFromFile( const std::string& _path );
		virtual void update( double _deltaTime ) override;
		virtual void draw  ( Context* _context, GraphicsDevice* _device ) override;

		IMaterial* m_material; /// TODO: change to index?
	private:
		Primitive* m_primitive;
	};
}