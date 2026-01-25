#pragma once

#include <wv/string.h>
#include <wv/debug/error.h>

#include <vector>

namespace wv {

typedef uint64_t TypeUUID;

struct TypeInfo
{
	const char* name;
	TypeUUID typeUUID;
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

private:
	static inline ReflectionRegistry* s_instance = nullptr;

	std::vector<TypeInfo*> m_typeInfos;
};

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
Ty* tryCast( IReflectedType* _type )
{
	static_assert( std::is_base_of<IReflectedType, Ty>() );

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

#define WV_REFLECT_TYPE( _typename, _base ) \
public: \
typedef InheritenceRegister<_base> Inheritence_t; \
static inline const wv::TypeInfo* s_typeInfo = wv::ReflectionRegistry::getSingleton()->registerType( #_typename ); \
static constexpr const char* getStaticTypeName() { return #_typename; } \
static wv::TypeUUID getStaticTypeUUID() { return s_typeInfo->typeUUID; } \
virtual std::string getTypeName() const override { return _typename::getStaticTypeName(); } \
virtual wv::TypeUUID getTypeUUID() const override { return _typename::getStaticTypeUUID(); } \
virtual std::vector<TypeUUID> getInheritedUUIDs() const override { return Inheritence_t::getRecursiveTypeUUIDs(); } 
