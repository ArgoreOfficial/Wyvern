#pragma once

#include <typeinfo>
#include <vector>

namespace arx {

template<typename _Ty> struct ret_of;
template<typename _Ty> struct ret_of<_Ty*> : ret_of<_Ty> {};
template<typename _Rty, typename... _Args> 
struct ret_of<_Rty( _Args... )> { 
	using Ret = _Rty; 
};

template<auto _F, typename _Fpty = decltype( _F )>
class reflected_function
{
public:
	typedef std::vector<const char*> str_vec_t;

	reflected_function() :
		fptr{ _F },
		m_name{ symbol() }
	{ }

	reflected_function( const char* _name ) :
		fptr{ _F },
		m_name{ _name }
	{ }

	constexpr const char*     symbol   ( void ) const { return typeid( _Fpty ).name(); }
	constexpr const char*     name     ( void ) const { return m_name; }
	constexpr const char*     ret_name ( void ) const { return typeid( ret_of<_Fpty>::Ret ).name(); }
	constexpr const size_t    arg_count( void ) const { return _arg_count( _F ); }
	constexpr const str_vec_t arg_names( void ) const { return _arg_names( _F ); }

	template<typename... _Args>
	auto operator()( _Args... _args ) {
		return fptr( _args... );
	}

private:
	
	_Fpty fptr;
	const char* m_name;

	template<typename _Rty, typename... _Args>
	constexpr const size_t _arg_count( _Rty( * )( _Args... ) ) const {
		return sizeof...( _Args );
	}

	template<typename _Rty, typename... _Args>
	constexpr const std::vector<const char*> _arg_names( _Rty( * )( _Args... ) ) const {
		return std::vector<const char*>{ typeid( _Args ).name()... };
	}
};

}
