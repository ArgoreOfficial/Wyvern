#include "Node.h"

///////////////////////////////////////////////////////////////////////////////////////

wv::Node::Node( const uint64_t& _uuid, const std::string& _name ):
	m_uuid{ _uuid },
	m_name{ _name }
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Node::~Node()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Node::addChild( Node* _node )
{
	if ( !_node )
		return;
	for ( size_t i = 0; i < m_children.size(); i++ )
		if ( m_children[ i ] == _node ) return; // node already has child
	m_children.push_back( _node );

	_node->m_parent = this;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Node::removeChild( Node* _node )
{
	if ( !_node )
		return;

	for ( size_t i = 0; i < m_children.size(); i++ )
	{
		if ( m_children[ i ] != _node )
			continue;

		m_children.erase( m_children.begin() + i );
		_node->m_parent = nullptr;
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Node::moveChild( Node* _node, Node* _newParent )
{
	if ( !_node || !_newParent )
		return;

	removeChild( _node );
	_newParent->addChild( _node );
}
