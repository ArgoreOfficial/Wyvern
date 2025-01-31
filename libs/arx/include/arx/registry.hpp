#pragma once

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <type_traits>

namespace arx
{

template<typename _Ty>
class registry
{
public:
	typedef _Ty* ( *allocator_fptr_t )( void* );

	static int32_t add_entry( const std::string& _name, allocator_fptr_t _pAllocator ) {
		g_allocators[ _name ] = _pAllocator;
		return g_allocators.size();
	}

	static _Ty* alloc( const std::string& _name, void* _pUserData = nullptr ) {
		return g_allocators.at( _name )( _pUserData );
	}

private:
	static inline std::unordered_map<std::string, allocator_fptr_t> g_allocators{};

};

template<typename _Ty, typename _Pty>
class registar : public _Pty
{
public:
	static_assert( std::is_base_of_v<registry<_Pty>, _Pty>, "_Pty must derive from registry<_Pty>" );
	//static_assert( std::is_convertible_v<_Pty*, registry<_Pty>*>, "_Pty must inherit registry<_Pty> as public" );
	//static_assert( std::is_convertible_v<_Ty*, registar<_Ty, _Pty>*>, "_Ty must inherit registar<_Ty, _Pty> as public" );

	struct entry
	{
		typedef _Pty* ( *allocator_fptr_t )( void* );

	private:
		template<typename _Cty>
		static allocator_fptr_t get_allocator( decltype( &_Cty::alloc ) ) {
			return _Cty::alloc;
		}

		template<typename _Cty>
		static allocator_fptr_t get_allocator( ... ) {
			return []( void* )
				{
					return reinterpret_cast<_Pty*>( new _Cty{} );
				};
		}

	public:
		entry() {
			auto ptr = get_allocator<_Ty>( 0 );
			id = _Pty::add_entry( _Ty::get_name(), ptr );
		}

		int32_t id = -1;
	};

private:
	static inline registar<_Ty, _Pty>::entry g_entry{};
};

}
