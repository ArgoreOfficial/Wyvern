#include "object.h"

#include <wv/runtime/query.h>
#include <wv/runtime/properties.h>
#include <wv/runtime/callable.h>

wv::IRuntimeProperty* wv::IRuntimeObject::getPropertyImpl( const std::string& _property )
{
	IRuntimeProperty* ptr = pQuery->pProperties->getPtr( _property );
    return ptr;
}

wv::IRuntimeCallable* wv::IRuntimeObject::getFunctionImpl( const std::string& _function )
{
    IRuntimeCallable* ptr = pQuery->pFunctions->getPtr( _function );
    return ptr;
}

bool wv::IRuntimeObject::hasProperty( const std::string& _property )
{
	IRuntimeProperty* ptr = pQuery->pProperties->getPtr( _property );
	return ptr != nullptr;
}

void wv::IRuntimeObject::setPropertyStr( const std::string& _property, const std::string& _valueStr )
{
    IRuntimeProperty* ptr = getPropertyImpl( _property );
	if( ptr == nullptr )
		return; // error

    switch( ptr->type )
    {
    case RuntimeMemberType::kInt:    setProperty( _property, wv::stot<int>( _valueStr ) );   break;
    case RuntimeMemberType::kFloat:  setProperty( _property, wv::stot<float>( _valueStr ) ); break;
    case RuntimeMemberType::kString: setProperty( _property, _valueStr );                    break;
    }
}

void wv::IRuntimeObject::callFunction( const std::string& _function, const std::vector<std::string>& _args )
{
	IRuntimeCallable* callable = getFunctionImpl( _function );
	if ( callable == nullptr )
		return; // error

	callable->call( this, _args );
}
