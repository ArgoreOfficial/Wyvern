#pragma once

#include <wv/reflection.h>

namespace wv {

struct ScriptComponent
{
	std::string path;
	std::string script;

	static inline wv::Reflection reflection{
		wv::reflect( "path", &ScriptComponent::path )
	};
};

}