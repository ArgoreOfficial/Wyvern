#pragma once

#include <string>

namespace arx {

template<typename _Ty> struct typename_of;

template<typename _Ty>
struct typename_of<_Ty*> { 
	using Ty = _Ty; 
};

template<typename _Ty, typename _Mty>
struct typename_of<_Ty _Mty::*> { 
	using Ty = _Ty; 
};

template<typename _Ty, auto _Ty::* ptr>
constexpr const char* member_name() {
	return typeid( typename_of<decltype( ptr )>::Ty ).name();
}

enum typeval {
	type,
	type_pointer,
	function,
	member_variable,
	member_function,
};

template<typename _Ty> struct typeval_of {
	static const typeval ty = type;
};

template<typename _Ty> struct typeval_of<_Ty*> : typeval_of<_Ty> {
	static const typeval ty = type_pointer;
};

template<typename _Rty, typename... _Args>
struct typeval_of<_Rty( * )( _Args... )> {
	static const typeval ty = function;
};

template<typename _Ty, typename _Rty, typename... _Args>
struct typeval_of<_Rty( _Ty::* )( _Args... )> {
	static const typeval ty = member_function;
};

template<typename _Ty, typename _Mty>
struct typeval_of<_Ty _Mty::*> {
	static const typeval ty = member_variable;
};



template<typename T>
static T str_to_T( const char* _str ) {
	return static_cast<T>( str_to_T<int32_t>( _str ) );
}

template<> static const char* str_to_T<const char*>( const char* _str ) { return _str; }
template<> static int32_t     str_to_T<int32_t>    ( const char* _str ) { return std::stoi( _str ); }
template<> static float       str_to_T<float>      ( const char* _str ) { return std::stof( _str ); }


}
