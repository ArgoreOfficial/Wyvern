#pragma once

#include <wv/types.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <wv/math/transform.h>

#include <filesystem>
#include <type_traits>

namespace wv {
class IReflectedType;
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
		return std::formatter<std::string>::format( _v.string(), _ctx );
	}
};

template <typename Ty>
struct formatter<wv::Vector2<Ty>> : std::formatter<std::string>
{
	auto format( wv::Vector2<Ty> _v, format_context& _ctx ) const {
		return std::formatter<std::string>::format( std::format( "[{}, {}]", _v.x, _v.y ), _ctx );
	}
};

template <typename Ty>
struct formatter<wv::Vector3<Ty>> : std::formatter<std::string>
{
	auto format( wv::Vector3<Ty> _v, format_context& _ctx ) const {
		return std::formatter<std::string>::format( std::format( "[{}, {}, {}]", _v.x, _v.y, _v.z ), _ctx );
	}
};

template <typename Ty>
struct formatter<wv::Vector4<Ty>> : std::formatter<std::string>
{
	auto format( wv::Vector4<Ty> _v, format_context& _ctx ) const {
		return std::formatter<std::string>::format( std::format( "[{}, {}, {}, {}]", _v.x, _v.y, _v.z, _v.w ), _ctx );
	}
};

template <typename Ty>
struct formatter<wv::Transform<Ty>> : std::formatter<std::string>
{
	auto format( wv::Transform<Ty> _v, format_context& _ctx ) const {
		return std::formatter<std::string>::format( std::format( "[{}, {}, {}]", _v.position, _v.rotation, _v.scale ), _ctx );
	}
};

}
