#include "ReflectionRegistry.h"

#include <wv/Debug/Print.h>
#include <wv/Reflection/ReflectedClass.h>

///////////////////////////////////////////////////////////////////////////////////////

int wv::cReflectionRegistry::reflectClass( const std::string& _name, iClassOperator* _operator )
{
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Reflecting '%s'\n", _name.c_str() );

	tReflectedClassesMap& classes = wv::cReflectionRegistry::getClasses();

	classes[ _name ] = { _name, _operator };
	
	return classes.size();
}

///////////////////////////////////////////////////////////////////////////////////////

void* wv::cReflectionRegistry::createInstance( const std::string& _name )
{
	tReflectedClassesMap& classes = wv::cReflectionRegistry::getClasses();

	if( !classes.contains( _name ) )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Class '%s' does not exist or hasn't been reflected\n", _name.c_str() );
		return nullptr;
	}

	sClassReflection& op = classes[ _name ];
	return op.pOperator->createInstance();
}

///////////////////////////////////////////////////////////////////////////////////////

void* wv::cReflectionRegistry::createInstanceYaml( const std::string& _name, fkyaml::node& _yaml )
{
	tReflectedClassesMap& classes = wv::cReflectionRegistry::getClasses();

	if( !classes.contains( _name ) )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Class '%s' does not exist or hasn't been reflected\n", _name.c_str() );
		return nullptr;
	}

	sClassReflection& op = classes[ _name ];
	return op.pOperator->createInstanceYaml( _yaml );
}

wv::tReflectedClassesMap& wv::cReflectionRegistry::getClasses()
{
	static tReflectedClassesMap classes{};
	return classes;
}
