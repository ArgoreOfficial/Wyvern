#pragma once

#include <unordered_map>
#include <string>

#include <auxiliary/json/json11.hpp>
#include <wv/memory/Function.h>

#include <auxiliary/json/json11.hpp>

#include <wv/math/vector3.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IClassOperator;

///////////////////////////////////////////////////////////////////////////////////////

	struct ClassReflection
	{
		std::string name = "";
		IClassOperator* pOperator = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	/// TODO: move
	
	using Json = json11::Json; // dummy

	Vector3f jsonToVec3( const Json::array& _arr );

	struct ParseData
	{
		wv::Json json;
	};
	
///////////////////////////////////////////////////////////////////////////////////////

	typedef std::unordered_map<std::string, ClassReflection> ReflectedClassesMap_t;

///////////////////////////////////////////////////////////////////////////////////////

	class ReflectionRegistry
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
		static typename wv::Function<C*, ParseData&>::fptr_t get_parseInstance_impl( decltype( &C::parseInstance ) ) {
			return &C::parseInstance; 
		}
		template<typename C> 
		static typename wv::Function<C*, ParseData&>::fptr_t get_parseInstance_impl( ... ) {
			return nullptr; 
		}

	public:
		static int reflectClass( const std::string& _name, IClassOperator* _operator );

		static void* createInstance( const std::string& _name );
		static void* parseInstance ( const std::string& _name, ParseData& _data );

		template<typename C> 
		static typename wv::Function<C*>::fptr_t get_createInstance( void ) { 
			return get_createInstance_impl<C>( 0 ); 
		};

		template<typename C> 
		static typename wv::Function<C*, ParseData&>::fptr_t get_parseInstance( void ) {
			return get_parseInstance_impl<C>( 0 ); 
		};

		// classes map has to be local static because 
		// global static does not guarantee it to be 
		// created before REFLECT_CLASS() is called
		static ReflectedClassesMap_t& getClasses();

///////////////////////////////////////////////////////////////////////////////////////

	};

}