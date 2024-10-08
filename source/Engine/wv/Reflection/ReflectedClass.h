#pragma once

#include <wv/Memory/Function.h>
#include <wv/Reflection/ReflectionRegistry.h>
#include <wv/Debug/Print.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iClassOperator
	{
	public:
		virtual ~iClassOperator() { };

		virtual void* createInstance( void )              = 0;
		virtual void* parseInstance ( sParseData& _data ) = 0;

	protected:
		std::string m_name;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	class cReflectedClass : public iClassOperator
	{
	public:
		cReflectedClass( const std::string& _name ) { m_name = _name; }
		virtual ~cReflectedClass() { }

		virtual void* createInstance( void ) override
		{
			if( m_createInstance.m_fptr )
				return m_createInstance();

			printf( "%s::createInstance not implemented!\n", m_name.c_str() );
			return nullptr;
		}

		virtual void* parseInstance( sParseData& _data ) override
		{
			if( m_parseInstance.m_fptr )
				return m_parseInstance( _data );

			printf( "%s::parseInstance not implemented!\n", m_name.c_str() );
			return nullptr;
		}

		typename wv::Function<T*>                  m_createInstance;
		typename wv::Function<T*, wv::sParseData&> m_parseInstance;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	struct ClassReflect
	{
		ClassReflect( const std::string _name )
		{
			static cReflectedClass<T> classReflection{ _name };
			classReflection.m_createInstance.bind( cReflectionRegistry::get_createInstance<T>() );
			classReflection.m_parseInstance.bind( cReflectionRegistry::get_parseInstance<T>() );
			cReflectionRegistry::reflectClass( _name, static_cast< iClassOperator* >( &classReflection ) );
			
		#ifdef WV_DEBUG
			if( classReflection.m_createInstance.m_fptr ) printf( "    ::createInstance\n" );
			if( classReflection.m_parseInstance.m_fptr  ) printf( "    ::parseInstance\n" );
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