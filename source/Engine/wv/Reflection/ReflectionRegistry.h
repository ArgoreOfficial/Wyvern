#pragma once

#include <unordered_map>
#include <string>

#include <wv/Auxiliary/json/json11.hpp>
#include <wv/Memory/Function.h>

#include <wv/Auxiliary/json/json11.hpp>

#include <wv/Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iClassOperator;

///////////////////////////////////////////////////////////////////////////////////////

	struct sClassReflection
	{
		std::string name = "";
		iClassOperator* pOperator = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	/// TODO: move
	
	using Json = json11::Json; // dummy

	static cVector3f jsonToVec3( const Json::array& _arr )
	{
		return cVector3f(
			static_cast<float>( _arr[ 0 ].number_value() ),
			static_cast<float>( _arr[ 1 ].number_value() ),
			static_cast<float>( _arr[ 2 ].number_value() )
		);
	}

	struct sParseData
	{
		wv::Json json;
	};
	
///////////////////////////////////////////////////////////////////////////////////////

	typedef std::unordered_map<std::string, sClassReflection> tReflectedClassesMap;

///////////////////////////////////////////////////////////////////////////////////////

	class cReflectionRegistry
	{

	private:
		/// TODO: generalize?

		template<typename C> 
		static typename wv::Function<C*>::fptr_t get_createInstance_impl( decltype( &C::createInstance ) ) { 
			return &C::createInstance; 
		}
		template<typename C> 
		static typename wv::Function<C*>::fptr_t get_createInstance_impl( ... ) { 
			return nullptr; 
		}

		template<typename C> 
		static typename wv::Function<C*, sParseData&>::fptr_t get_parseInstance_impl( decltype( &C::parseInstance ) ) {
			return &C::parseInstance; 
		}
		template<typename C> 
		static typename wv::Function<C*, sParseData&>::fptr_t get_parseInstance_impl( ... ) {
			return nullptr; 
		}

	public:
		static int reflectClass( const std::string& _name, iClassOperator* _operator );

		static void* createInstance( const std::string& _name );
		static void* parseInstance ( const std::string& _name, sParseData& _data );

		template<typename C> 
		static typename wv::Function<C*>::fptr_t get_createInstance( void ) { 
			return get_createInstance_impl<C>( 0 ); 
		};

		template<typename C> 
		static typename wv::Function<C*, sParseData&>::fptr_t get_parseInstance( void ) {
			return get_parseInstance_impl<C>( 0 ); 
		};

		// classes map has to be local static because 
		// global static does not guarantee it to be 
		// created before REFLECT_CLASS() is called
		static tReflectedClassesMap& getClasses();

///////////////////////////////////////////////////////////////////////////////////////

	};

}