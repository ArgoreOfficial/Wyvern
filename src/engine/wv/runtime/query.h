#pragma once

#include <stdio.h>
#include <vector>
#include <string>

namespace wv {

struct RuntimeFunctions;
struct RuntimeProperties;
struct IRuntimeObject;

struct IRuntimeQuery
{
	IRuntimeQuery( const std::string& _name, const std::string& _base ) :
		name{ _name },
		base{ _base }
	{}
	std::string name;
	std::string base;

	IRuntimeObject* ( *fptrConstruct )( void ) = nullptr;
	IRuntimeObject* ( *fptrSafeCast  )( void* ) = nullptr;

	RuntimeFunctions*  pFunctions  = nullptr;
	RuntimeProperties* pProperties = nullptr;

	template<typename _Ty>
	static int instantiate() {
		static _Ty inst{};
		return 1;
	}

	void dump();
};

}
