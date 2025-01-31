#pragma once

#include <stdint.h>
#include <arx/ptr.hpp>

namespace arx
{

template<typename _Ty>
struct ptr_reloc
{
	ptr_reloc( uintptr_t _offset ) :
		ptr{ reinterpret_cast<_Ty*>( arx::getBaseAddr() + _offset ) }
	{ }

	_Ty operator->() { return *ptr; }
	_Ty operator*()  { return *ptr; }

	_Ty* ptr;
};

template<typename _Rty, typename... _Args>
struct ptr_reloc<_Rty( _Args... )>
{
	typedef _Rty( *fptr_t )( _Args... );

	ptr_reloc( uintptr_t _offset ) :
		fptr{ reinterpret_cast<fptr_t>( arx::getBaseAddr() + _offset ) }
	{ }

	_Rty operator()( _Args... _args ) {
		return fptr( _args... );
	}

	fptr_t fptr;
};

}