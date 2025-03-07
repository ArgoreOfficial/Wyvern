#include "runtime_object.h"

#include <wv/runtime/query.h>
#include <wv/runtime/runtime_properties.h>

uint8_t wv::IRuntimeObjectBase::* wv::IRuntimeObjectBase::getPropertyImpl( const std::string& _property )
{
    uint8_t IRuntimeObjectBase::* ptr = pQuery->pProperties->getPtr( _property );
    return ptr;
}
