#include "reflection.h"

#include <wv/math/math.h>

wv::ReflectionRegistry* wv::ReflectionRegistry::getSingleton()
{
	if ( !s_instance )
		s_instance = new ReflectionRegistry();

	return s_instance;
}

void wv::ReflectionRegistry::destroySingleton()
{
	if ( !s_instance )
		return;

	delete s_instance;
	s_instance = nullptr;
}

wv::TypeInfo* wv::ReflectionRegistry::registerType( const char* _name )
{
	if ( _name == nullptr )
		return nullptr;

	for ( size_t i = 0; i < m_typeInfos.size(); i++ )
		if ( std::strcmp( _name, m_typeInfos[ i ]->name ) == 0 )
			return nullptr; // already exists
	
	TypeInfo* info = new TypeInfo();

	info->name = _name;
	info->typeUUID = wv::Math::randomU64();

	m_typeInfos.push_back( info );

	return info;
}
