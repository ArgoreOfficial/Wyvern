#pragma once

#include <wv/Memory/Function.h>
#include <wv/Reflection/ReflectionRegistry.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iClassOperator
	{
	public:
		virtual void* createInstance    ( void )               { printf( "%s::createInstance not implemented!",     m_name.c_str() ); return nullptr; }
		virtual void* createInstanceJson( nlohmann::json _js ) { printf( "%s::createInstanceJson not implemented!", m_name.c_str() ); return nullptr; }

	protected:
		std::string m_name;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	class cReflectedClass : public iClassOperator
	{
	public:
		cReflectedClass( const std::string& _name ) { m_name = _name; }

		virtual void* createInstance    ( void )               override { return T::createInstance(); }
		virtual void* createInstanceJson( nlohmann::json _js ) override { return T::createInstanceJson( _js ); }

	private:
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	struct ClassReflect
	{
		ClassReflect( const std::string _name )
		{
			static cReflectedClass<T> classReflection{ _name };
			cReflectionRegistry::reflectClass( _name, static_cast< iClassOperator* >( &classReflection ) );
		}
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	class ClassReflector
	{
	public:
		static ClassReflect<T> creator;
	};

}