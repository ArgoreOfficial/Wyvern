#pragma once

#include <wv/Reflection/ReflectedClass.h>

///////////////////////////////////////////////////////////////////////////////////////

#define REFLECT_CLASS( _class ) \
wv::ClassReflect<_class> wv::ClassReflector<_class>::creator{ #_class }

///////////////////////////////////////////////////////////////////////////////////////

#define GENERATE_REFLECTED_CLASS_FUNCS( _class ) \
public: \
static _class* createInstance    ( void )                  { return nullptr; } \
static _class* createInstanceJson( nlohmann::json& _json ) { return nullptr; }

