#include "reflection_registry.h"

#include <wv/debug/log.h>
#include <wv/reflection/reflected_class.h>

///////////////////////////////////////////////////////////////////////////////////////

int wv::ReflectionRegistry::reflectClass( const std::string& _name, IClassOperator* _operator )
{
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Reflecting '%s'\n", _name.c_str() );

	ReflectedClassesMap_t& classes = wv::ReflectionRegistry::getClasses();

	ClassReflection c;
	c.name = _name;
	c.pOperator = _operator;
	classes[ _name ] = c;
	
	return classes.size();
}

///////////////////////////////////////////////////////////////////////////////////////

void* wv::ReflectionRegistry::createInstance( const std::string& _name )
{
	ReflectedClassesMap_t& classes = wv::ReflectionRegistry::getClasses();

	auto search = classes.find( _name );
	if( search == classes.end() )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Class '%s' does not exist or hasn't been reflected\n", _name.c_str() );
		return nullptr;
	}

	ClassReflection& op = classes[ _name ];
	return op.pOperator->createInstance();
}

///////////////////////////////////////////////////////////////////////////////////////

void* wv::ReflectionRegistry::parseInstance( const std::string& _name, ParseData& _data )
{
	ReflectedClassesMap_t& classes = wv::ReflectionRegistry::getClasses();

	auto search = classes.find( _name );
	if( search == classes.end() )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Class '%s' does not exist or hasn't been reflected\n", _name.c_str() );
		return nullptr;
	}

	ClassReflection& op = classes[ _name ];
	return op.pOperator->parseInstance( _data );
}

wv::ReflectedClassesMap_t& wv::ReflectionRegistry::getClasses()
{
	static ReflectedClassesMap_t classes{};
	return classes;
}

wv::Vector3f wv::jsonToVec3( const Json::array& _arr )
{
	return wv::Vector3f(
		static_cast<float>( _arr[ 0 ].number_value() ),
		static_cast<float>( _arr[ 1 ].number_value() ),
		static_cast<float>( _arr[ 2 ].number_value() )
	);
}
