#pragma once

#include <wv/runtime/query.h>
#include <wv/runtime/callable.h>
#include <wv/runtime/properties.h>

#include <vector>
#include <string>

namespace wv {

template<typename _Ty> static wv::IRuntimeQuery* getRuntimeGlobal() { return nullptr; }

struct IRuntimeQuery;
struct IRuntimeCallable;

class IRuntimeObject
{
protected:
	IRuntimeProperty*         getPropertyImpl   ( const std::string& _property );
	IRuntimeCallable*         getFunctionImpl   ( const std::string& _property );

	template<typename _Ty>
	_Ty IRuntimeObject::* getPropertyPtrImpl( const std::string& _property ) {
		RuntimeMemberProperty<_Ty>* ptr = ( RuntimeMemberProperty<_Ty>* )pQuery->pProperties->getPtr( _property );
		if( ptr )
		   return ptr->ptr;
		return nullptr;
	}

public:
	IRuntimeQuery* pQuery = nullptr;

	virtual ~IRuntimeObject() {}

	template<typename _Ty>
	_Ty getProperty( const std::string& _property ) {
		_Ty IRuntimeObject::* ptr = getPropertyPtrImpl<_Ty>( _property );
		if ( !ptr )
			return _Ty{};

		_Ty ret = ( this->*ptr );
		return ret;
	}

	template<typename _Ty>
	_Ty* getPropertyPtr( const std::string& _property ) {
		_Ty IRuntimeObject::* ptr = getPropertyPtrImpl<_Ty>( _property );
		if ( !ptr )
			return nullptr;

		_Ty* ret = &( this->*ptr );
		return ret;
	}

	template<typename _Ty>
	void setProperty( const std::string& _property, const _Ty& _value ) {
		_Ty IRuntimeObject::* ptr = getPropertyPtrImpl<_Ty>( _property );
		if( ptr == nullptr )
			return; // error

		( this->*ptr ) = _value;
	}

	bool hasProperty( const std::string& _property );

	void setPropertyStr( const std::string& _property, const std::string& _valueStr );
	void callFunction( const std::string& _function, const std::vector<std::string>& _args );
	void callFunction( const std::string& _function ) {
		callFunction( _function, {} );
	}
};

class RuntimeObject : public IRuntimeObject
{
public:
	RuntimeObject() {
		//pQuery = getRuntimeGlobal<_Ty>();
	}
};

}