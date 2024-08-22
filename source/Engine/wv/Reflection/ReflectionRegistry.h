#pragma once

#include <unordered_map>
#include <string>

#include <wv/Auxiliary/json.hpp>

namespace wv
{
	class iClassOperator;

	struct sClassReflection
	{
		std::string name;
		iClassOperator* pOperator;
	};

	class cReflectionRegistry
	{
	public:
		static inline std::unordered_map<std::string, sClassReflection> m_classes;

		static int reflectClass( const std::string& _name, iClassOperator* _operator );

		static void* createClassInstance( const std::string& _name );
		static void* createClassInstanceJson( const std::string& _name, nlohmann::json& _json );

	};
}