#pragma once

#include <wv/runtime/query.h>
#include <wv/runtime/callable.h>

#include <vector>
#include <string>

namespace wv {

template<typename _Ty> static wv::IRuntimeQuery* getRuntimeGlobal() { return nullptr; }

struct IRuntimeQuery;
struct IRuntimeCallable;
struct RuntimeMemberProperty;

class IRuntimeObject
{
protected:
	RuntimeMemberProperty*    getPropertyImpl   ( const std::string& _property );
	uint8_t IRuntimeObject::* getPropertyPtrImpl( const std::string& _property );
	IRuntimeCallable*         getFunctionImpl   ( const std::string& _property );

public:
	IRuntimeQuery* pQuery = nullptr;

	template<typename _Ty>
	_Ty getProperty( const std::string& _property ) {
		uint8_t IRuntimeObject::* ptr = getPropertyPtrImpl( _property );
		_Ty ret = ( this->*ptr );
		return ret;
	}

	template<typename _Ty>
	_Ty* getPropertyPtr( const std::string& _property ) {
		uint8_t IRuntimeObject::* ptr = getPropertyPtrImpl( _property );
		_Ty* ret = reinterpret_cast<_Ty*>( &( this->*ptr ) );
		return ret;
	}

	template<typename _Ty>
	void setProperty( const std::string& _property, const _Ty& _value ) {
		uint8_t IRuntimeObject::* ptr = getPropertyPtrImpl( _property );
		if( ptr == nullptr )
			return; // error

		(_Ty&)( this->*ptr ) = _value;
	}

	void setPropertyStr( const std::string& _property, const std::string& _valueStr );

	void callFunction( const std::string& _function, const std::vector<std::string>& _args ) {
		IRuntimeCallable* callable = getFunctionImpl( _function );
		if( callable == nullptr )
			return; // error

		callable->call( this, _args );
	}

	void callFunction( const std::string& _function ) {
		callFunction( _function, {} );
	}
};

template<typename _Ty>
class RuntimeObject : public IRuntimeObject
{
public:
	RuntimeObject() {
		pQuery = getRuntimeGlobal<_Ty>();
	}
};

}