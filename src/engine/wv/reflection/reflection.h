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
	static void fromJson( const nlohmann::json& _json, Ty& _v ) { 
		if ( _json.is_null() )
			return;

		_json.get_to( _v ); 
	}
};

template<>
struct SerializeField<UUID>
{
	static nlohmann::json toJson( const UUID& _v ) { return (uint64_t)_v; }
	static void fromJson( const nlohmann::json& _json, UUID& _v ) { 
		if ( _json.is_null() )
			return;

		uint64_t v;
		_json.get_to( v );
		_v = v;
	}
};

template<typename Ty>
struct SerializeField<Vector2<Ty>>
{
	static nlohmann::json toJson( const Vector2<Ty>& _v ) { return std::vector<Ty>{ _v.x, _v.y }; }
	static void fromJson( const nlohmann::json& _json, Vector2<Ty>& _v ) { 
		if ( _json.is_null() )
			return;

		std::vector<Ty> vec;
		_json.get_to( vec );
		_v = { vec[ 0 ], vec[ 1 ] };
	}
};

template<typename Ty>
struct SerializeField<Vector3<Ty>>
{
	static nlohmann::json toJson( const Vector3<Ty>& _v ) { return std::vector<Ty>{ _v.x, _v.y, _v.z }; }
	static void fromJson( const nlohmann::json& _json, Vector3<Ty>& _v ) {
		if ( _json.is_null() )
			return;

		std::vector<Ty> vec;
		_json.get_to( vec );
		_v = { vec[ 0 ], vec[ 1 ], vec[ 2 ] };
	}
};

template<typename Ty>
struct SerializeField<Vector4<Ty>>
{
	static nlohmann::json toJson( const Vector4<Ty>& _v ) { return std::vector<Ty>{ _v.x, _v.y, _v.z, _v.w }; }
	static void fromJson( const nlohmann::json& _json, Vector4<Ty>& _v ) {
		if ( _json.is_null() )
			return;

		std::vector<Ty> vec;
		_json.get_to( vec );
		_v = { vec[ 0 ], vec[ 1 ], vec[ 2 ], vec[ 3 ] };
	}
};

template<typename Ty>
struct SerializeField<Rotor<Ty>>
{
	static nlohmann::json toJson( const Rotor<Ty>& _v ) { return std::vector<Ty>{ _v.s, _v.xy, _v.xz, _v.yz }; }
	static void fromJson( const nlohmann::json& _json, Rotor<Ty>& _v ) {
		if ( _json.is_null() )
			return;

		std::vector<Ty> vec;
		_json.get_to( vec );
		_v = { vec[ 0 ], vec[ 1 ], vec[ 2 ], vec[ 3 ] };
	}
};

template<typename Ty>
struct SerializeField<Ref<Ty>>
{
	static nlohmann::json toJson( const Ref<Ty>& _v ) { 
		if ( !_v ) return {};

		return _v->getPath().string();
	}

	static void fromJson( const nlohmann::json& _json, Ref<Ty>& _v ) {
		if ( _json.is_null() )
			return;

		std::string path;
		_json.get_to( path );
		_v = Ty::get( path );
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

				if ( !j.is_null() )
					jvec.push_back( j );
			}
			else
			{
				nlohmann::json j = SerializeField<Ty>::toJson( v );
				
				if( !j.is_null() )
					jvec.push_back( j );
			}
		}

		return jvec;
	}

	static void fromJson( const nlohmann::json& _json, std::vector<Ty>& _v ) {
		for ( auto& [k, v] : _json.items() )
		{
			Ty tmp;
			SerializeField<Ty>::fromJson( v, tmp );
			_v.push_back( tmp );
		}
	}
};

struct IField
{
	const char* name;

	constexpr IField( const char* _name ) : name{ _name } { }

	virtual nlohmann::json serialize( const void* _cptr ) = 0;
	virtual void deserialize( const nlohmann::json& _json, void* _cptr ) = 0;
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

			for ( IField* f : reflection_of<FieldTy>.fields )
				j[ f->name ] = f->serialize( &( cptr->*fptr ) );

			return j;
		}
		else
		{
			return SerializeField<FieldTy>::toJson( cptr->*fptr );
		}
	}

	virtual void deserialize( const nlohmann::json& _json, void* _cptr ) override {
		Class* cptr = (Class*)_cptr;
		if constexpr ( HasReflection<FieldTy>::value )
		{
			for ( IField* f : reflection_of<FieldTy>.fields )
				f->deserialize( _json[ f->name ], &( cptr->*fptr ));
		}
		else
		{
			return SerializeField<FieldTy>::fromJson( _json, cptr->*fptr );
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

	void deserialize( const nlohmann::json& _json, void* _cptr ) const {
		for ( const auto& r : fields )
			r->deserialize( _json[ r->name ], _cptr );
	}
};

// Helpers

namespace Serialize {

template<typename Ty>
static nlohmann::json toJson( const Ty& _v ) {
	if constexpr ( HasReflection<Ty>::value )
		return reflection_of<Ty>.serialize( &_v );
	else
		return SerializeField<Ty>::toJson( _v );
}

template<typename Ty>
static void fromJson( const nlohmann::json& _json, Ty& _v ) {
	if constexpr ( HasReflection<Ty>::value )
		return reflection_of<Ty>.deserialize( _json, &_v );
	else
		return SerializeField<Ty>::fromJson( _json, _v );
}

}

}