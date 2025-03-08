#include "registry.h"

wv::IRuntimeObject* wv::RuntimeRegistry::instantiate( const std::string& _objectName )
{
	if( m_queries.count( _objectName ) == 0 )
		return nullptr; // error

	return m_queries[ _objectName ]->fptrConstruct();
}
