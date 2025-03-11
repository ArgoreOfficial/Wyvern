#pragma once

#include <wv/runtime/registry.h>
#include <wv/string.h>

#define WV_RUNTIME_OBJECT( _object, _base ) \
class _object; \
namespace _object##Query { template<typename _Ty> struct RuntimeQuery; }; \
static const int g##_object##QueryInstance = wv::IRuntimeQuery::instantiate<_object##Query::RuntimeQuery<_object>>(); \
template<> wv::IRuntimeQuery* wv::getRuntimeGlobal<_object>() { return wv::RuntimeRegistry::get()->m_queries[ #_object ]; } \
template<> inline constexpr const char* wv::type<_object>() { return #_object; } \
namespace _object##Query { \
template<typename _Ty> \
struct RuntimeQuery : public wv::IRuntimeQuery { \
	RuntimeQuery() : wv::IRuntimeQuery( #_object, #_base ) { \
		wv::RuntimeRegistry::get()->registerRuntime<_object>( (wv::IRuntimeQuery*)this ); \
	} }; }
