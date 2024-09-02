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
		virtual void* createInstance    ( void )                = 0;
		virtual void* createInstanceYaml( fkyaml::node& _data ) = 0;

	protected:
		std::string m_name;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	class cReflectedClass : public iClassOperator
	{
	public:
		cReflectedClass( const std::string& _name ) { m_name = _name; }

		virtual void* createInstance( void ) override
		{
			if( m_createInstance.m_fptr )
				return m_createInstance();

			printf( "%s::createInstance not implemented!\n", m_name.c_str() );
			return nullptr;
		}

		virtual void* createInstanceYaml( fkyaml::node& _data ) override
		{
			if( m_createInstanceYaml.m_fptr )
				return m_createInstanceYaml( _data );

			printf( "%s::createInstanceYaml not implemented!\n", m_name.c_str() );
			return nullptr;
		}

		wv::Function<T*>                m_createInstance = nullptr;
		wv::Function<T*, fkyaml::node&> m_createInstanceYaml = nullptr;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	struct ClassReflect
	{
		ClassReflect( const std::string _name )
		{
			static cReflectedClass<T> classReflection{ _name };
			classReflection.m_createInstance.bind( cReflectionRegistry::get_createInstance<T>() );
			classReflection.m_createInstanceYaml.bind( cReflectionRegistry::get_createInstanceYaml<T>() );
			cReflectionRegistry::reflectClass( _name, static_cast< iClassOperator* >( &classReflection ) );
			
		#ifdef WV_DEBUG
			if( classReflection.m_createInstance.m_fptr )     printf( "    ::createInstance\n" );
			if( classReflection.m_createInstanceYaml.m_fptr ) printf( "    ::createInstanceYaml\n" );
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