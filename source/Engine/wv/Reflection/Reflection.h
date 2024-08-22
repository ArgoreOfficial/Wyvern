#pragma once

#include <wv/Reflection/ReflectedClass.h>

#define REFLECT_CLASS( _class ) \
wv::ClassReflect<_class> wv::ClassReflector<_class>::creator{ #_class }
