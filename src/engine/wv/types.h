#pragma once

#include <wv/helpers/strong_type.hpp>
#include <wv/resource_id.h>
#include <wv/string.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

#define WV_DEFINE_ID( _c ) typedef wv::strong_type<uint16_t, struct _c##_t> _c

namespace wv {

///////////////////////////////////////////////////////////////////////////////////////

enum class TopologyClass
{
	WV_POINT = 1,
	WV_LINE,
	WV_TRIANGLE
};

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
using Ref = std::shared_ptr<Ty>;

template<typename Ty>
using WeakRef = std::weak_ptr<Ty>;

///////////////////////////////////////////////////////////////////////////////////////

class UUID
{
public:
	UUID() : m_value{ 0 } { };
	UUID( uint64_t _value ) : m_value{ _value } { };

	std::string toString() const {
		return std::format( "{:x}", m_value );
	}

	bool     operator==( const wv::UUID& _other ) const { return m_value == _other.m_value; }
	bool     operator!=( const wv::UUID& _other ) const { return m_value != _other.m_value; }
	bool     operator> ( const wv::UUID& _other ) const { return m_value > _other.m_value; }
	bool     operator< ( const wv::UUID& _other ) const { return m_value < _other.m_value; }
	uint64_t operator- ( const wv::UUID& _other ) const { return m_value - _other.m_value; }
	uint64_t operator+ ( const wv::UUID& _other ) const { return m_value + _other.m_value; }

	operator uint64_t() const { return m_value; }

private:
	uint64_t m_value;
};

///////////////////////////////////////////////////////////////////////////////////////

}
