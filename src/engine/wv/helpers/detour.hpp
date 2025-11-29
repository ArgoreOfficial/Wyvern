#pragma once

#include <wv/ptr.hpp>

namespace wv {

template<typename _Ty> struct fptr_detour;
template<typename _Rty, typename... _Args>
struct fptr_detour<_Rty( _Args... )> { 
	typedef _Rty( *fptr_t )( _Args... );

	fptr_detour()
	{ }

	fptr_detour( uintptr_t _targetOffset ) : target{ reinterpret_cast<fptr_t>( wv::getBaseAddr() + _targetOffset ) }
	{ }

	fptr_detour( wchar_t* _pModule, char* _pProcName, fptr_t _fpDetour ):
		pModule{ _pModule },
		pProcName{ _pProcName },
		detour{ _fpDetour }
	{ }
	
	void createHook( wchar_t* _wstrModule, char* _strProcName, fptr_t _pDetour ) {
		//MH_CreateHookApiEx( _pszModule, _pszProcName, (LPVOID)_pDetour, (LPVOID*)&original, NULL );
	}
		
	void createHook() {
		//MH_CreateHookApiEx( pszModule, pszProcName, (LPVOID)detour, (LPVOID*)&original, NULL );
	}

	void createHook( void* _pTarget, void* _pDetour ) {
		//MH_CreateHook( pTarget, pDetour, (LPVOID*)&original );
	}
	
	void createHook( fptr_t pDetour ) {
		//MH_CreateHook( target, reinterpret_cast<LPVOID>( pDetour ), (LPVOID*)&original );
	}

	fptr_t target = nullptr;
	fptr_t original = nullptr;
	
	wchar_t* pModule   = nullptr;
	char*    pProcName = nullptr;
	fptr_t detour = nullptr;
};

}
