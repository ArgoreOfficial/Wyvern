#pragma once

#include <stdint.h>

namespace wv {
	
class ResourceID
{
public:
	ResourceID() :
		value{ InvalidID }
	{ }

	explicit ResourceID( uint16_t _v ) :
		value{ _v }
	{ }

	ResourceID( const ResourceID& _other ) : 
		value{ _other.value } 
	{ }

	inline void invalidate() { value = InvalidID; }
	inline bool isValid() const { return value != InvalidID; }

	bool operator==( const ResourceID& _other ) const { return value == _other.value; }
	bool operator!=( const ResourceID& _other ) const { return value != _other.value; }
	bool operator> ( const ResourceID& _other ) const { return value > _other.value; }
	bool operator< ( const ResourceID& _other ) const { return value < _other.value; }
	uint16_t operator- ( const ResourceID& _other ) const { return value - _other.value; }
	uint16_t operator+ ( const ResourceID& _other ) const { return value + _other.value; }

	operator uint16_t() const { return value; }

	static constexpr uint16_t InvalidID = 0;
	uint16_t value = InvalidID;
};

}