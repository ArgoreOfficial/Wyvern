#pragma once

#include <unordered_map>
#include <string>

#include <wv/Auxiliary/json.hpp>

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
	public:

		static int reflectClass( const std::string& _name, iClassOperator* _operator );

		static void* createInstance    ( const std::string& _name );
		static void* createInstanceJson( const std::string& _name, nlohmann::json& _json );

		// classes map has to be local static because 
		// global static does not guarantee it to be 
		// created before REFLECT_CLASS() is called
		static tReflectedClassesMap& getClasses();

///////////////////////////////////////////////////////////////////////////////////////

	};

}