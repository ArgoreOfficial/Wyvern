#pragma once

#include <wv/string.h>
#include <wv/debug/error.h>

#include <vector>

namespace wv {

typedef uint64_t TypeUUID;

struct IMemberTypeInfo
{
	const char* name;
	const char* displayName;
	size_t offset;
	virtual std::string format( void* _class ) = 0;
};

template<typename Ty>
struct MemberTypeInfo : IMemberTypeInfo
{
	virtual std::string format( void* _class ) override {
		WV_ASSERT( _class != nullptr );

		Ty* v = reinterpret_cast<Ty*>( (char*)_class + offset );
		return std::format( "{}", *v );
	}
};

struct TypeInfo
{
	const char* name;
	TypeUUID typeUUID;
	std::vector<IMemberTypeInfo*> members;
};

template<typename Ty>
void vectorConcat( std::vector<Ty>& _a, const std::vector<Ty>& _b ) {
	_a.insert( _a.end(), _b.begin(), _b.end() );
}

template<typename... Ty>
struct InheritenceRegister
{
	static inline std::vector<TypeUUID> s_cachedRecursiveTypeUUIDs;

	static std::vector<TypeUUID> getTypeUUIDs() {
		return { Ty::getStaticTypeUUID()... };
	}

	static std::vector<TypeUUID> getRecursiveTypeUUIDs() {
		if ( s_cachedRecursiveTypeUUIDs.size() == 0 )
		{
			s_cachedRecursiveTypeUUIDs = { Ty::getStaticTypeUUID()... };
			vectorConcat( s_cachedRecursiveTypeUUIDs, Ty::Inheritence_t::getRecursiveTypeUUIDs()... );
		}
		
		return s_cachedRecursiveTypeUUIDs;
	}
};

template<>
struct InheritenceRegister<void>
{
	static std::vector<TypeUUID> getTypeUUIDs() { return {}; }
	static std::vector<TypeUUID> getRecursiveTypeUUIDs() { return {}; }
};

class IReflectedType
{
public:
	typedef InheritenceRegister<void> Inheritence_t;

	virtual std::string           getTypeName()       const = 0;
	virtual TypeUUID              getTypeUUID()       const = 0;
	virtual std::vector<TypeUUID> getInheritedUUIDs() const = 0;

	static wv::TypeUUID getStaticTypeUUID() { return 0; }
	
private:
	
};

class ReflectionRegistry
{
public:
	ReflectionRegistry() = default;
	
	/*
	 * This is the only singleton created inside a
	 * getSingleton() function.
	 * 
	 * The reason is because the reflection system will be accessed while 
	 * static variables are being initialized, so cannot be created within main()
	 */

	static ReflectionRegistry* getSingleton();
	static void destroySingleton();

	static bool hasInitialized() { return s_instance != nullptr; }

	TypeInfo* registerType( const char* _name );

	template<typename Ty>
	int registerMember( size_t _offset, const char* _name, const char* _displayName ) {
		WV_ASSERT( m_lastTypeInfo != nullptr );

		MemberTypeInfo<Ty>* type = new MemberTypeInfo<Ty>();
		type->name        = _name;

		if ( 0 == strcmp( _displayName, "" ) )
			type->displayName = _name;
		else
			type->displayName = _displayName;

		type->offset      = _offset;

		m_lastTypeInfo->members.push_back( type );

		return m_lastTypeInfo->members.size() - 1;
	}

	TypeInfo* getTypeInfo( TypeUUID _typeUUID ) {
		for ( TypeInfo* typeInfo : m_typeInfos )
		{
			if ( typeInfo->typeUUID == _typeUUID )
				return typeInfo;
		}

		return nullptr;
	}

private:
	static inline ReflectionRegistry* s_instance = nullptr;

	TypeInfo* m_lastTypeInfo = nullptr;

	std::vector<TypeInfo*> m_typeInfos;
};

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
Ty* tryCast( IReflectedType* _type )
{
	static_assert( std::is_base_of<IReflectedType, Ty>(), "Type must be a valid IReflectedType" );
	static_assert( std::is_same_v<decltype( &Ty::getStaticTypeUUID ), wv::TypeUUID( * )( )>, "Type must define WV_REFLECT_TYPE" );
	
	if ( _type == nullptr )
		return nullptr;

	const TypeUUID toUUID = Ty::getStaticTypeUUID();

	// exact match, return immediately
	if ( _type->getTypeUUID() == toUUID )
		return static_cast<Ty*>( _type );

	// check inherited types
	const std::vector<TypeUUID> inheritence = _type->getInheritedUUIDs();
	for( const TypeUUID uuid : inheritence )
		if( uuid == toUUID )
			return static_cast<Ty*>( _type );
	
	return nullptr;
}

}

///////////////////////////////////////////////////////////////////////////////////////

// helper
namespace wv {
static wv::ReflectionRegistry* reflreg() { return wv::ReflectionRegistry::getSingleton(); }
}

#define WV_REFLECT_TYPE( _typename, _base ) \
public: \
typedef _typename ClassType; \
typedef wv::InheritenceRegister<_base> Inheritence_t; \
static inline const wv::TypeInfo* s_typeInfo = wv::reflreg()->registerType( #_typename ); \
static constexpr const char* getStaticTypeName() { return #_typename; } \
static wv::TypeUUID getStaticTypeUUID() { return s_typeInfo->typeUUID; } \
virtual std::string getTypeName() const override { return _typename::getStaticTypeName(); } \
virtual wv::TypeUUID getTypeUUID() const override { return _typename::getStaticTypeUUID(); } \
virtual std::vector<wv::TypeUUID> getInheritedUUIDs() const override { return Inheritence_t::getRecursiveTypeUUIDs(); } 

#define WV_REFLECT_MEMBER( _member, ... ) \
static inline const int _member##_reflect = wv::reflreg()->registerMember<decltype( _member )>( offset_of<ClassType, decltype( _member ), &ClassType::##_member>(), #_member, "" __VA_ARGS__ );

template <typename T, typename R, R T::* M>
constexpr std::size_t offset_of()
{
	return reinterpret_cast<std::size_t>( &( ( (T*)0 )->*M ) );
}

