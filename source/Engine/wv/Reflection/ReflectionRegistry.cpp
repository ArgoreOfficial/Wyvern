#include "ReflectionRegistry.h"

#include <wv/Debug/Print.h>
#include <wv/Reflection/ReflectedClass.h>

///////////////////////////////////////////////////////////////////////////////////////

int wv::cReflectionRegistry::reflectClass( const std::string& _name, iClassOperator* _operator )
{
	m_classes[ _name ] = { _name, _operator };
	return m_classes.size();
}

///////////////////////////////////////////////////////////////////////////////////////

void* wv::cReflectionRegistry::createInstance( const std::string& _name )
{
	if( !m_classes.contains( _name ) )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Class '%s' does not exist or hasn't been reflected\n", _name.c_str() );
		return nullptr;
	}

	sClassReflection& op = m_classes[ _name ];
	return op.pOperator->createInstance();
}

///////////////////////////////////////////////////////////////////////////////////////

void* wv::cReflectionRegistry::createInstanceJson( const std::string& _name, nlohmann::json& _json )
{
	if( !m_classes.contains( _name ) )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Class '%s' does not exist or hasn't been reflected\n", _name.c_str() );
		return nullptr;
	}

	sClassReflection& op = m_classes[ _name ];
	return op.pOperator->createInstanceJson( _json );
}
