#pragma once

#include <wv/Reflection/ReflectedClass.h>

///////////////////////////////////////////////////////////////////////////////////////

#define REFLECT_CLASS( _class ) \
wv::ClassReflect<_class> wv::ClassReflector<_class>::creator{ #_class }

///////////////////////////////////////////////////////////////////////////////////////

#define REFLECTED_CLASS_FUNCS( _class ) \
static _class* createInstance    ( void );\
static _class* createInstanceJson( nlohmann::json& _json );

