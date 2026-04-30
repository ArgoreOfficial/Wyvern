#pragma once

#include <wv/string.h>
#include <wv/debug/error.h>
#include <wv/format.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

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
	nlohmann::json operator()( Ty& _fptr ) { return _fptr; }
};

template<typename Ty>
struct SerializeField<Vector2<Ty>>
{
	nlohmann::json operator()( Vector2<Ty>& _fptr ) { return std::vector<Ty>{ _fptr.x, _fptr.y }; }
};

template<typename Ty>
struct SerializeField<Vector3<Ty>>
{
	nlohmann::json operator()( Vector3<Ty>& _fptr ) { return std::vector<Ty>{ _fptr.x, _fptr.y, _fptr.z }; }
};

template<typename Ty>
struct SerializeField<Vector4<Ty>>
{
	nlohmann::json operator()( Vector4<Ty>& _fptr ) { return std::vector<Ty>{ _fptr.x, _fptr.y, _fptr.z, _fptr.w }; }
};

template<typename Ty>
struct SerializeField<Ref<Ty>>
{
	nlohmann::json operator()( Ref<Ty>& _fptr ) { 
		if ( !_fptr ) return {};

		return _fptr->getPath().string();
	}
};

template<typename Ty>
struct SerializeField<std::vector<Ty>>
{
	nlohmann::json operator()( std::vector<Ty>& _fptr ) {
		std::vector<nlohmann::json> j;
		for ( Ty& v : _fptr )
			j.push_back( SerializeField<Ty>()( v ) );
		return j;
	}
};

struct IField
{
	const char* name;

	constexpr IField( const char* _name ) : name{ _name } { }

	virtual nlohmann::json serialize( void* _ptr ) = 0;
};

template <typename Class, typename FieldTy>
struct Field : IField
{
	FieldTy Class::* fptr;

	constexpr Field() noexcept = default;
	constexpr Field( const char* _name, FieldTy Class::* _p ) noexcept : IField{ _name }, fptr{ _p } { }

	virtual nlohmann::json serialize( void* _cptr ) override {
		Class* cptr = (Class*)_cptr;
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
			return SerializeField<FieldTy>()( cptr->*fptr );
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

	nlohmann::json serialize( void* _ptr ) const {
		nlohmann::json j{};
		for ( auto& r : fields )
			j[ r->name ] = r->serialize( _ptr );
		return j;
	}
};

}