#pragma once

#include <wv/reflection/reflected_class.h>

///////////////////////////////////////////////////////////////////////////////////////

#define REFLECT_CLASS( _class ) template<> wv::ClassReflect<_class> wv::ClassReflector<_class>::creator{ #_class }

///////////////////////////////////////////////////////////////////////////////////////

#define REFLECTED_CLASS_FUNCS( _class ) \
static _class* createInstance    ( void );\
static _class* parseInstance( nlohmann::json& _json );

