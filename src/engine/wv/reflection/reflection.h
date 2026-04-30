#pragma once

#include <wv/string.h>
#include <wv/debug/error.h>

#include <wv/format.h>

#include <vector>

namespace wv {

}

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

template <typename T, typename R, R T::* M>
constexpr std::size_t offset_of()
{
	return reinterpret_cast<std::size_t>( &( ( (T*)0 )->*M ) );
}

template <typename B = void, typename C, typename M>
constexpr size_t offset_of2( M C::* _p )
{
	using Base = typename std::conditional<std::is_same_v<B, void>, C, B>::type;
	return (char*)&( ( Base* )nullptr->*_p ) - ( char* )nullptr;
}

}