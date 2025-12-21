#pragma once

#include <wv/string.h>

#include <vector>

namespace wv {

struct TypeInfo
{
	const char* name;
	uint64_t typeUUID;
};

class IReflectedType
{
public:
	virtual std::string getTypeName() const = 0;
	virtual uint64_t    getTypeUUID() const = 0;
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
Ty* tryCast( IReflectedType* _type );

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
Ty* tryCast( IReflectedType* _type )
{
	static_assert( std::is_base_of<IReflectedType, Ty>() );

	if ( _type == nullptr )
		return nullptr;

	// require exact match, this should ideally check a chain of derivation
	if ( _type->getTypeName() != wv::typeName<Ty>() )
		return nullptr;

	return reinterpret_cast<Ty*>( _type );
}

}

///////////////////////////////////////////////////////////////////////////////////////

#define WV_REFLECT_TYPE( _typename ) \
public: \
static inline const TypeInfo* s_typeInfo = wv::ReflectionRegistry::getSingleton()->registerType( #_typename );\
static constexpr const char* typeName() { return #_typename; } \
static uint64_t typeUUID() { return s_typeInfo->typeUUID; } \
virtual std::string getTypeName() const override { return _typename::typeName(); } \
virtual uint64_t getTypeUUID()    const override { return _typename::typeUUID(); }
