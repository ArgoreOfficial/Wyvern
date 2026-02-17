#pragma once

#include <wv/helpers/strong_type.hpp>
#include <wv/resource_id.h>
#include <wv/string.h>

#include <string>
#include <filesystem>

///////////////////////////////////////////////////////////////////////////////////////

#define WV_DEFINE_ID( _c ) typedef wv::strong_type<uint16_t, struct _c##_t> _c

namespace wv {

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
		return std::format( "0x{:x}", m_value );
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

namespace std {

template <>
struct hash<wv::UUID>
{
	size_t operator()( const wv::UUID& _type ) const { return hash<uint64_t>()( _type ); }
};

template <>
struct formatter<wv::UUID> : std::formatter<std::string>
{
	auto format( wv::UUID _uuid, format_context& _ctx ) const {
		return std::formatter<std::string>::format( _uuid.toString(), _ctx );
	}
};

template <>
struct formatter<std::filesystem::path> : std::formatter<std::string>
{
	auto format( std::filesystem::path _v, format_context& _ctx ) const {
		return std::formatter<std::string>::format( _v.string(), _ctx);
	}
};

}
