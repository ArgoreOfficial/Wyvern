#pragma once

#include <string>

namespace wv {

template <typename _Ty>
static _Ty stot( const std::string& _str );

template<> int         stot( const std::string& _str ) { return std::stoi ( _str ); }
template<> double      stot( const std::string& _str ) { return std::stod ( _str ); }
template<> float       stot( const std::string& _str ) { return std::stof ( _str ); }
template<> long        stot( const std::string& _str ) { return std::stol ( _str ); }
template<> long long   stot( const std::string& _str ) { return std::stoll( _str ); }
template<> long double stot( const std::string& _str ) { return std::stold( _str ); }
template<> std::string stot( const std::string& _str ) { return _str; }

}
