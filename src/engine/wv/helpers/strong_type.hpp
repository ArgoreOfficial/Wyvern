#pragma once

#include <type_traits>

namespace wv {

template<typename _Ty, typename _Tag>
struct strong_type
{
	strong_type( void ) :
		value{ InvalidID }
	{ }

	strong_type( const _Ty& _v ) :
		value{ _v }
	{
	}

	strong_type( const strong_type<_Ty, _Tag>& _other ) :
		value{ _other.value }
	{}

	inline void invalidate() { value = InvalidID; }
	inline bool is_valid() const { return value != InvalidID; }

	bool operator==( const strong_type<_Ty, _Tag>& _other ) const { return value == _other.value; }
	bool operator!=( const strong_type<_Ty, _Tag>& _other ) const { return value != _other.value; }
	bool operator> ( const strong_type<_Ty, _Tag>& _other ) const { return value > _other.value; }
	bool operator< ( const strong_type<_Ty, _Tag>& _other ) const { return value < _other.value; }
	_Ty  operator- ( const strong_type<_Ty, _Tag>& _other ) const { return value - _other.value; }
	_Ty  operator+ ( const strong_type<_Ty, _Tag>& _other ) const { return value + _other.value; }

	operator _Ty() const { return value; }

	static constexpr _Ty InvalidID = static_cast<_Ty>( 0 );
	_Ty value = InvalidID;
};

}

namespace std {

template <typename T, typename Tag>
struct hash<wv::strong_type<T, Tag>>
{
	size_t operator()( const wv::strong_type<T, Tag>& _type ) const { return hash<T>()( _type.value ); }
};

}
