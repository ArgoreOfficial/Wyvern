#pragma once

#include <stdio.h>

#ifdef WV_PLATFORM_WINDOWS
#include <Windows.h>
#undef min // these need to be undefined to keep ::min and ::max functions from not dying...
#undef max 
#endif

namespace wv 
{
    namespace Debug
    {
		namespace Internal
		{
			static const char* LEVEL_STR[] = {
				"  ", // info
				"D ", // debug
				"* ", // warning
				"! ", // error
				"X "  // fatal
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

			static inline void SetPrintLevelColor( int _level )
			{
			#ifdef WV_PLATFORM_WINDOWS
				if( !hConsole )
					hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

				if ( _level < 0 )
					_level = 0; // default print

				SetConsoleTextAttribute( hConsole, LEVEL_COL[ _level ] );
			#endif
			}
		}

		enum PrintLevel
		{
			WV_PRINT_INFO = 0,
			WV_PRINT_DEBUG,
			WV_PRINT_WARN,
			WV_PRINT_ERROR,
			WV_PRINT_FATAL
		};

		template<typename... Args>
        inline void Print( const char* _str, Args... _args )
        {
			printf( Internal::LEVEL_STR[ 0 ] );
			printf( _str, _args... );
        }

		template<typename... Args>
		inline void Print( PrintLevel _printLevel, const char* _str, Args... _args )
		{
		#ifndef WV_DEBUG
			if ( _printLevel == WV_PRINT_DEBUG )
				return;
		#endif

			Internal::SetPrintLevelColor( _printLevel );
			printf( Internal::LEVEL_STR[ _printLevel ] );
			printf( _str, _args... );
			Internal::SetPrintLevelColor( -1 );
		}
    }
}

