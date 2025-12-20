#pragma once

#include <wv/string.h>

namespace wv {

class IReflectedType
{
public:
	virtual std::string getTypeName() const = 0;
};

template<typename Ty>
Ty* tryCast( IReflectedType* _type )
{
	static_assert( std::is_base_of<IReflectedType, Ty>() );

	if ( _type == nullptr )
		return nullptr;

	// require exact match, this should ideally check a chain of derivation
	if ( _type->getTypeName() != wv::typeName<Ty>() )
		return nullptr;

	return reinterpret_cast<Ty*>( _type );
}

}

#define WV_REFLECT_TYPE( _typename ) \
public: \
static constexpr const char* typeName() { return #_typename; } \
virtual std::string getTypeName() const override { return typeName(); }
