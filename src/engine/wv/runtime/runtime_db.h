#pragma once

#include <wv/runtime/runtime_properties.h>
#include <wv/runtime/query.h>
#include <wv/runtime/runtime_object.h>

namespace wv {

class RuntimeDataBase
{
public:
	static RuntimeDataBase* get() {
		static RuntimeDataBase inst{};
		return &inst;
	}
	/* 
	template<typename _Ty> static void queryMethodsImpl( decltype( &_Ty::queryMethods ) ) { return _Ty::queryMethods(); }
	template<typename _Ty> static void queryMethodsImpl( ... ) { return {}; }
	*/

	void dump();


	template<typename _Ty> 
	static void queryPropertiesImpl( ... ) { }
	template<typename _Ty> 
	static void queryPropertiesImpl( RuntimeProperties* p, decltype( &_Ty::queryProperties ) ) {
		_Ty::queryProperties( p );
	}

	template<typename _Ty>
	void registerRuntime( IRuntimeQuery* _pRtQuery )
	{
		_pRtQuery->pMethods = new RuntimeMethods();
		// queryMethodsImpl<_Ty>( _pRtQuery->pMethods, 0 );
		
		_pRtQuery->pProperties = new RuntimeProperties();
		queryPropertiesImpl<_Ty>( _pRtQuery->pProperties, 0 );

		m_queries.emplace( _pRtQuery->name, _pRtQuery );
	}

	std::unordered_map<std::string, IRuntimeQuery*> m_queries;
};

}
