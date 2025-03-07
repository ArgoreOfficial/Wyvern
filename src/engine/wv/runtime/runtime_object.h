#pragma once

#include <wv/runtime/query.h>

#include <string>

namespace wv {

template<typename _Ty> wv::IRuntimeQuery* getRuntimeGlobal();

class IRuntimeQuery;

class IRuntimeObjectBase
{
public:
	IRuntimeQuery* pQuery = nullptr;

protected:
	uint8_t IRuntimeObjectBase::* getPropertyImpl( const std::string& _property );
};

template<typename _Ty>
class IRuntimeObject : public IRuntimeObjectBase
{
public:
	IRuntimeObject() {
		pQuery = getRuntimeGlobal<_Ty>();
	}

	template<typename _Ty>
	_Ty getProperty( const std::string& _property ) {
		uint8_t IRuntimeObjectBase::* ptr = getPropertyImpl( _property );
		_Ty ret = ( this->*ptr );
		return ret;
	}

	template<typename _Ty>
	void setProperty( const std::string& _property, const _Ty& _value ) {
		uint8_t IRuntimeObjectBase::* ptr = getPropertyImpl( _property );
		if( ptr == nullptr )
			return;

		(_Ty&)(this->*ptr) = _value;
	}

};

}