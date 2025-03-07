#include "object.h"

#include <wv/runtime/query.h>
#include <wv/runtime/properties.h>
#include <wv/runtime/callable.h>

uint8_t wv::IRuntimeObject::* wv::IRuntimeObject::getPropertyImpl( const std::string& _property )
{
    uint8_t IRuntimeObject::* ptr = pQuery->pProperties->getPtr( _property );
    return ptr;
}

wv::IRuntimeCallableBase* wv::IRuntimeObject::getFunctionImpl( const std::string& _function )
{
    IRuntimeCallableBase* ptr = pQuery->pFunctions->getPtr( _function );
    return ptr;
}
