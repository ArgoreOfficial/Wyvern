#include "object.h"

#include <wv/runtime/query.h>
#include <wv/runtime/properties.h>
#include <wv/runtime/callable.h>

wv::RuntimeMemberProperty* wv::IRuntimeObject::getPropertyImpl( const std::string& _property )
{
    RuntimeMemberProperty* ptr = pQuery->pProperties->getPtr( _property );
    return ptr;
}

uint8_t wv::IRuntimeObject::* wv::IRuntimeObject::getPropertyPtrImpl( const std::string& _property )
{
    RuntimeMemberProperty* ptr = pQuery->pProperties->getPtr( _property );
    if( ptr )
        return ptr->ptr;
    return nullptr;
}

wv::IRuntimeCallable* wv::IRuntimeObject::getFunctionImpl( const std::string& _function )
{
    IRuntimeCallable* ptr = pQuery->pFunctions->getPtr( _function );
    return ptr;
}

void wv::IRuntimeObject::setPropertyStr( const std::string& _property, const std::string& _valueStr )
{
    RuntimeMemberProperty* ptr = getPropertyImpl( _property );
	if( ptr == nullptr )
		return; // error

    switch( ptr->type )
    {
    case RuntimeMemberType::kInt:    setProperty( _property, wv::stot<int>( _valueStr ) );   break;
    case RuntimeMemberType::kFloat:  setProperty( _property, wv::stot<float>( _valueStr ) ); break;
    case RuntimeMemberType::kString: setProperty( _property, _valueStr );                    break;
    }
}
