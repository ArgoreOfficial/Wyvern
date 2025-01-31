#pragma once

#include <Windows.h>

namespace arx
{

class func_loader
{
public:
	func_loader( LPCWSTR _module ) {
		hModule = GetModuleHandleW( _module );
	}

	template<auto _Fn, typename _Ty = decltype( _Fn )>
	_Ty get( const char* _fn ) {
		return (_Ty)GetProcAddress( hModule, _fn );
	}

private:
	HMODULE hModule;
};

}