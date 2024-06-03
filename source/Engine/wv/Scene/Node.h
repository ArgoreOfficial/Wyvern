#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <wv/Math/Transform.h>

namespace wv
{
	class Context;
	class GraphicsDevice;

	class Node
	{
	public:
		 Node( const uint64_t& _uuid, const std::string& _name );
		~Node();

		void addChild   ( Node* _node );
		void removeChild( Node* _node );
		void moveChild  ( Node* _node, Node* _newParent );

		virtual void update( double _deltaTime ) 
		{
			for ( size_t i = 0; i < m_children.size(); i++ )
				m_children[ i ]->update( _deltaTime );
		}

		virtual void draw( Context* _context, GraphicsDevice* _device ) 
		{
			for ( size_t i = 0; i < m_children.size(); i++ )
				m_children[ i ]->draw( _context, _device );
		}

		Transformf m_transform;
		Transformf m_worldTransform;

	protected:

		uint64_t    m_uuid;
		std::string m_name;
		
		Node* m_parent = nullptr;
		std::vector<Node*> m_children{};

	};
}

// REFLECTNODE( INode )