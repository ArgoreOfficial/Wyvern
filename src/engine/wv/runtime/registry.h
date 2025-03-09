#pragma once

#include <wv/runtime/properties.h>
#include <wv/runtime/callable.h>

#include <wv/runtime/query.h>
#include <wv/runtime/object.h>

#include <wv/memory/memory.h>

namespace wv {

namespace Runtime {

// creates object and initializes runtime query
template<typename _Ty> static _Ty* create() {
	IRuntimeObject* ptr = WV_NEW( _Ty );
	ptr->pQuery = getRuntimeGlobal<_Ty>();
	return static_cast<_Ty*>( ptr );
}

}

class RuntimeRegistry
{
public:
	static RuntimeRegistry* get() {
		static RuntimeRegistry inst{};
		return &inst;
	}

	void dump() {
		for ( auto& query : m_queries )
			query.second->dump();
	}

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

		_pRtQuery->fptrConstruct = []() -> IRuntimeObject* 
			{ 
				_Ty* p = WV_NEW_NAMED( _Ty, "IRuntimeObject" );
				p->pQuery = getRuntimeGlobal<_Ty>();;
				return (IRuntimeObject*)p;
			};
		
		_pRtQuery->fptrSafeCast = []( void* _ptr ) -> IRuntimeObject*
			{
				_Ty* pthis = reinterpret_cast<_Ty*>( _ptr );
				return static_cast<IRuntimeObject*>( pthis );
			};

		_pRtQuery->fptrGetQuery = []() -> IRuntimeQuery*
			{
				return getRuntimeGlobal<_Ty>();
			};

		m_queries.emplace( _pRtQuery->name, _pRtQuery );
	}

	IRuntimeObject* instantiate( const std::string& _objectName );
	IRuntimeObject* safeCast( const std::string& _objectName, void* _pObject );
	
	bool isRuntimeType( const std::string& _name ) {
		return m_queries.count( _name ) != 0;
	}

	std::unordered_map<std::string, IRuntimeQuery*> m_queries;
};

}
