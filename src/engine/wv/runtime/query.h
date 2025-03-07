#pragma once

namespace wv {

struct RuntimeFunctions;
struct RuntimeProperties;
struct IRuntimeObject;

struct IRuntimeQuery
{
	IRuntimeQuery( const char* _name, const char* _base ) :
		name{ _name },
		base{ _base }
	{}
	const char* name;
	const char* base;

	IRuntimeObject* ( *fptrConstruct )( void ) = nullptr;

	RuntimeFunctions*  pFunctions  = nullptr;
	RuntimeProperties* pProperties = nullptr;

	template<typename _Ty>
	static int instantiate() {
		static _Ty inst{};
		return 1;
	}
};

}
