#pragma once

#include <wv/memory/function.h>
#include <wv/reflection/reflection_registry.h>
#include <wv/debug/log.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IClassOperator
	{
	public:
		virtual ~IClassOperator() { };

		virtual void* createInstance( void )              = 0;
		virtual void* parseInstance ( ParseData& _data ) = 0;

	protected:
		std::string m_name;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	class ReflectedClass : public IClassOperator
	{
	public:
		ReflectedClass( const std::string& _name ) { m_name = _name; }
		virtual ~ReflectedClass() { }

		virtual void* createInstance( void ) override
		{
			if( m_createInstance.m_fptr )
				return m_createInstance();

			wv::Debug::Print( "%s::createInstance not implemented!\n", m_name.c_str() );
			return nullptr;
		}

		virtual void* parseInstance( ParseData& _data ) override
		{
			if( m_parseInstance.m_fptr )
				return m_parseInstance( _data );

			wv::Debug::Print( "%s::parseInstance not implemented!\n", m_name.c_str() );
			return nullptr;
		}

		typename wv::Function<T*>                  m_createInstance;
		typename wv::Function<T*, wv::ParseData&> m_parseInstance;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	struct ClassReflect
	{
		ClassReflect( const std::string _name )
		{
			static ReflectedClass<T> classReflection{ _name };
			classReflection.m_createInstance.bind( ReflectionRegistry::get_createInstance<T>() );
			classReflection.m_parseInstance.bind( ReflectionRegistry::get_parseInstance<T>() );
			ReflectionRegistry::reflectClass( _name, static_cast< IClassOperator* >( &classReflection ) );
			
		#ifdef WV_DEBUG
			if( classReflection.m_createInstance.m_fptr ) wv::Debug::Print( "    ::createInstance\n" );
			if( classReflection.m_parseInstance.m_fptr  ) wv::Debug::Print( "    ::parseInstance\n" );
		#endif

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