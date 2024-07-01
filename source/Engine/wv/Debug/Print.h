#pragma once

#include <stdio.h>
#include <mutex>

#ifdef WV_PLATFORM_WINDOWS
#include <Windows.h>
#undef min // these need to be undefined to keep ::min and ::max functions from not dying...
#undef max 
#endif

///////////////////////////////////////////////////////////////////////////////////////

namespace wv::Debug
{

	namespace Internal
	{

///////////////////////////////////////////////////////////////////////////////////////

		static const char* LEVEL_STR[] = {
			"          ", // info
			"DEBUG",
			" WARN",
			"ERROR",
			"FATAL" 
		};

		static int LEVEL_COL[] = {
			7,  // info
			11,  // debug
			14, // warning
			4,  // error
			12  // fatal
		};

	#ifdef WV_PLATFORM_WINDOWS
		static HANDLE hConsole = nullptr;
	#endif

		static inline std::mutex PRINT_MUTEX{};

///////////////////////////////////////////////////////////////////////////////////////

		static inline void SetPrintLevelColor( int _level )
		{
		#ifdef WV_PLATFORM_WINDOWS
			if ( !hConsole )
				hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

			if ( _level < 0 )
				_level = 0; // default print

			SetConsoleTextAttribute( hConsole, LEVEL_COL[ _level ] );
		#endif
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

	enum PrintLevel
	{
		WV_PRINT_INFO  = 0x0,
		WV_PRINT_DEBUG = 0x1,
		WV_PRINT_WARN  = 0x2,
		WV_PRINT_ERROR = 0x3,
		WV_PRINT_FATAL = 0x4
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename... Args>
    inline void Print( const char* _str, Args... _args )
    {
		Internal::PRINT_MUTEX.lock();

		printf( "%s", Internal::LEVEL_STR[ 0 ] );
		printf( _str, _args... );
		
		Internal::PRINT_MUTEX.unlock();
    }

	template<typename... Args>
	inline void Print( PrintLevel _printLevel, const char* _str, Args... _args )
	{
		Internal::PRINT_MUTEX.lock();

	#ifndef WV_DEBUG
		if ( _printLevel == WV_PRINT_DEBUG )
		{
			Internal::PRINT_MUTEX.unlock();
			return;
		}
	#endif
		if ( _printLevel != WV_PRINT_INFO )
		{
			printf( "[ " );
			Internal::SetPrintLevelColor( _printLevel );
			printf( Internal::LEVEL_STR[ _printLevel ] );
			Internal::SetPrintLevelColor( -1 );
			printf( " ] " );
		}
		else
			printf( Internal::LEVEL_STR[ _printLevel ] );

		printf( _str, _args... );

		Internal::PRINT_MUTEX.unlock();
	}

}


