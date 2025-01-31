#pragma once

#include <type_traits>

namespace arx {

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

	inline void invalidate() { value = InvalidID; }
	inline bool is_valid() const { return value != InvalidID; }

	bool operator==( const strong_type<_Ty, _Tag>& _other ) const { return value == _other.value; }
	bool operator!=( const strong_type<_Ty, _Tag>& _other ) const { return value != _other.value; }
	bool operator> ( const strong_type<_Ty, _Tag>& _other ) const { return value > _other.value; }
	bool operator< ( const strong_type<_Ty, _Tag>& _other ) const { return value < _other.value; }
	_Ty  operator- ( const strong_type<_Ty, _Tag>& _other ) const { return value - _other.value; }
	_Ty  operator+ ( const strong_type<_Ty, _Tag>& _other ) const { return value + _other.value; }

	static constexpr _Ty InvalidID = static_cast<_Ty>( -1 );
	_Ty value;
};

}

namespace std {

template <typename T, typename Tag>
struct hash<arx::strong_type<T, Tag>>
{
	size_t operator()( const arx::strong_type<T, Tag>& _type ) const { return hash<T>()( _type.value ); }
};

}
