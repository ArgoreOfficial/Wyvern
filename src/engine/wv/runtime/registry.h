#pragma once

#include <wv/runtime/properties.h>
#include <wv/runtime/callable.h>

#include <wv/runtime/query.h>
#include <wv/runtime/object.h>

namespace wv {

class RuntimeRegistry
{
public:
	static RuntimeRegistry* get() {
		static RuntimeRegistry inst{};
		return &inst;
	}

	void dump();

	template<typename _Ty> 
	static void queryPropertiesImpl( ... ) { }
	template<typename _Ty> 
	static void queryPropertiesImpl( RuntimeProperties* p, decltype( &_Ty::queryProperties ) ) {
		_Ty::queryProperties( p );
	}


	template<typename _Ty>
	static void queryFunctionsImpl( ... ) {}
	template<typename _Ty>
	static void queryFunctionsImpl( RuntimeFunctions* f, decltype( &_Ty::queryFunctions ) ) {
		_Ty::queryFunctions( f );
	}

	template<typename _Ty>
	void registerRuntime( IRuntimeQuery* _pRtQuery )
	{
		_pRtQuery->pFunctions = new RuntimeFunctions();
		queryFunctionsImpl<_Ty>( _pRtQuery->pFunctions, 0 );
		
		_pRtQuery->pProperties = new RuntimeProperties();
		queryPropertiesImpl<_Ty>( _pRtQuery->pProperties, 0 );

		m_queries.emplace( _pRtQuery->name, _pRtQuery );
	}

	std::unordered_map<std::string, IRuntimeQuery*> m_queries;
};

}
