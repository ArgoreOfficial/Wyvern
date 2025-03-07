#pragma once

namespace wv {

class RuntimeFunctions;
class RuntimeProperties;

struct IRuntimeQuery
{
	IRuntimeQuery( const char* _name, const char* _base ) :
		name{ _name },
		base{ _base }
	{}
	const char* name;
	const char* base;

	RuntimeFunctions*  pMethods    = nullptr;
	RuntimeProperties* pProperties = nullptr;

	template<typename _Ty>
	static int instantiate() {
		static _Ty inst{};
		return 1;
	}
};

}
