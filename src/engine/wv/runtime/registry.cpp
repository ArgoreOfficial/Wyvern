#include "registry.h"

wv::IRuntimeObject* wv::RuntimeRegistry::instantiate( const std::string& _objectName )
{
	if( m_queries.count( _objectName ) == 0 )
		return nullptr; // error

	return m_queries[ _objectName ]->fptrConstruct();
}

wv::IRuntimeObject* wv::RuntimeRegistry::safeCast( const std::string& _objectName, void* _pObject )
{
	if ( m_queries.count( _objectName ) == 0 )
		return nullptr; // error

	return m_queries[ _objectName ]->fptrSafeCast( _pObject );
}
