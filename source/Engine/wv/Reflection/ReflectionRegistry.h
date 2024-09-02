#pragma once

#include <unordered_map>
#include <string>

#include <fkYAML/node.hpp>
#include <wv/Memory/Function.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iClassOperator;

///////////////////////////////////////////////////////////////////////////////////////

	struct sClassReflection
	{
		std::string name = "";
		iClassOperator* pOperator = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef std::unordered_map<std::string, sClassReflection> tReflectedClassesMap;

///////////////////////////////////////////////////////////////////////////////////////

	class cReflectionRegistry
	{

	private:
		template<typename C> static wv::Function<C*>::fptr_t get_createInstance_impl( decltype( &C::createInstance ) ) { return &C::createInstance; }
		template<typename C> static wv::Function<C*>::fptr_t get_createInstance_impl( ... )                            { return nullptr; }

		template<typename C> static wv::Function<C*, fkyaml::node&>::fptr_t get_createInstanceYaml_impl( decltype( &C::createInstanceYaml ) ) { return &C::createInstanceYaml; }
		template<typename C> static wv::Function<C*, fkyaml::node&>::fptr_t get_createInstanceYaml_impl( ... )                                { return nullptr; }


	public:
		static int reflectClass( const std::string& _name, iClassOperator* _operator );

		static void* createInstance    ( const std::string& _name );
		static void* createInstanceYaml( const std::string& _name, fkyaml::node& _yaml );

		template<typename C> 
		static wv::Function<C*>::fptr_t get_createInstance( void ) { return get_createInstance_impl<C>( 0 ); };

		template<typename C> 
		static wv::Function<C*, fkyaml::node&>::fptr_t get_createInstanceYaml( void ) { return get_createInstanceYaml_impl<C>( 0 ); };

		// classes map has to be local static because 
		// global static does not guarantee it to be 
		// created before REFLECT_CLASS() is called
		static tReflectedClassesMap& getClasses();

///////////////////////////////////////////////////////////////////////////////////////

	};

}