#pragma once

#include <wv/types.h>

#include <wv/debug/error.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/math/vector4.h>
#include <wv/math/rotor.h>

#include <nlohmann/json.hpp>

#include <vector>

namespace wv {

}

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

template <typename B = void, typename C, typename M>
constexpr size_t offset_of( M C::* _p )
{
	using Base = typename std::conditional<std::is_same_v<B, void>, C, B>::type;
	return (char*)&( ( Base* )nullptr->*_p ) - ( char* )nullptr;
}

template <typename Ty>
constexpr inline const auto& reflection_of = Ty::reflection;

template <typename Ty, typename = int>
struct HasReflection : std::false_type { };

template <typename Ty>
struct HasReflection <Ty, decltype( (void)Ty::reflection, 0 )> : std::true_type { };

template<typename Ty>
struct SerializeField
{
	static nlohmann::json toJson( const Ty& _v ) { return _v; }
};

template<>
struct SerializeField<UUID>
{
	static nlohmann::json toJson( const UUID& _v ) { return (uint64_t)_v; }
};

template<typename Ty>
struct SerializeField<Vector2<Ty>>
{
	static nlohmann::json toJson( const Vector2<Ty>& _v ) { return std::vector<Ty>{ _v.x, _v.y }; }
};

template<typename Ty>
struct SerializeField<Vector3<Ty>>
{
	static nlohmann::json toJson( const Vector3<Ty>& _v ) { return std::vector<Ty>{ _v.x, _v.y, _v.z }; }
};

template<typename Ty>
struct SerializeField<Vector4<Ty>>
{
	static nlohmann::json toJson( const Vector4<Ty>& _v ) { return std::vector<Ty>{ _v.x, _v.y, _v.z, _v.w }; }
};

template<typename Ty>
struct SerializeField<Rotor<Ty>>
{
	static nlohmann::json toJson( const Rotor<Ty>& _v ) { return std::vector<Ty>{ _v.s, _v.xy, _v.xz, _v.yz }; }
};

template<typename Ty>
struct SerializeField<Ref<Ty>>
{
	static nlohmann::json toJson( const Ref<Ty>& _v ) { 
		if ( !_v ) return {};

		return _v->getPath().string();
	}
};

template<typename Ty>
struct SerializeField<std::vector<Ty>>
{
	static nlohmann::json toJson( const std::vector<Ty>& _v ) {
		std::vector<nlohmann::json> jvec;

		for ( const Ty& v : _v )
		{
			if constexpr ( HasReflection<Ty>::value )
			{
				nlohmann::json j{};

				for ( const auto& r : reflection_of<Ty>.fields )
					j[ r->name ] = r->serialize( &v );

				jvec.push_back( j );
			}
			else
			{
				// primitive or unreflected type
				jvec.push_back( SerializeField<Ty>::toJson( v ) );
			}
		}

		return jvec;
	}
};

template<typename Ty>
struct SerializeField<std::vector<Ty*>>
{
	static nlohmann::json toJson( const std::vector<Ty*>& _v ) {
		std::vector<nlohmann::json> jvec;

		for ( const Ty* v : _v )
		{
			if constexpr ( HasReflection<Ty>::value )
			{
				nlohmann::json j{};

				for ( const auto& r : reflection_of<Ty>.fields )
					j[ r->name ] = r->serialize( v );

				jvec.push_back( j );
			}
			else
			{
				// primitive or unreflected type
				jvec.push_back( SerializeField<Ty>()( *v ) );
			}
		}

		return jvec;
	}
};

struct IField
{
	const char* name;

	constexpr IField( const char* _name ) : name{ _name } { }

	virtual nlohmann::json serialize( const void* _ptr ) = 0;
};

template <typename Class, typename FieldTy>
struct Field : IField
{
	FieldTy Class::* fptr;

	constexpr Field() noexcept = default;
	constexpr Field( const char* _name, FieldTy Class::* _p ) noexcept : IField{ _name }, fptr{ _p } { }

	virtual nlohmann::json serialize( const void* _cptr ) override {
		const Class* cptr = (Class*)_cptr;
		if constexpr ( HasReflection<FieldTy>::value )
		{
			nlohmann::json j{};

			for ( const auto& r : reflection_of<FieldTy>.fields )
				j[ r->name ] = r->serialize( &( cptr->*fptr ) );

			return j;
		}
		else
		{
			// primitive or unreflected type
			return SerializeField<FieldTy>::toJson( cptr->*fptr );
		}
	}
};

template<typename Class, typename FieldTy>
constexpr IField* reflect( const char* _name, FieldTy Class::* _p )
{
	Field<Class, FieldTy>* f = new Field<Class, FieldTy>( _name, _p );
	return (IField*)f;
}

struct Reflection
{
	std::vector<IField*> fields;

	Reflection( std::initializer_list<IField*> _l ) {
		fields.insert( fields.end(), _l.begin(), _l.end() );
	}

	~Reflection() {
		for ( IField* f : fields )
			delete f;
	}

	nlohmann::json serialize( const void* _ptr ) const {
		nlohmann::json j{};
		for ( const auto& r : fields )
			j[ r->name ] = r->serialize( _ptr );
		return j;
	}
};

// Helpers

namespace Serialize {

template<typename Ty>
static nlohmann::json toJson( const Ty& _v ) {
	return SerializeField<Ty>::toJson( _v );
}

/*
template<typename Ty>
static void fromJson( Ty& _v, const nlohmann::json& _json ) {
	SerializeField<Ty>::fromJson( _v, _json );
}
*/

}

}