#pragma once

#include <stdint.h>

namespace wv {

template<typename Ty, typename Tag = void>
class TResourceID
{
public:
	TResourceID() = default;

	explicit TResourceID( Ty _v ) :
		value{ _v }
	{ }

	TResourceID( const TResourceID<Ty, Tag>& _other ) :
		value{ _other.value } 
	{ }

	inline void invalidate() { value = InvalidID; }
	inline bool isValid() const { return value != InvalidID; }

	bool operator==( const TResourceID<Ty, Tag>& _other ) const { return value == _other.value; }
	bool operator!=( const TResourceID<Ty, Tag>& _other ) const { return value != _other.value; }
	bool operator> ( const TResourceID<Ty, Tag>& _other ) const { return value > _other.value; }
	bool operator< ( const TResourceID<Ty, Tag>& _other ) const { return value < _other.value; }
	Ty   operator- ( const TResourceID<Ty, Tag>& _other ) const { return value - _other.value; }
	Ty   operator+ ( const TResourceID<Ty, Tag>& _other ) const { return value + _other.value; }

	operator Ty() const { return value; }

	static constexpr Ty InvalidID = 0;
	Ty value = InvalidID;
};

using ResourceID = TResourceID<uint16_t, struct ResourceID_t>;

}