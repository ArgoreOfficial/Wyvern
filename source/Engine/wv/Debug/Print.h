#pragma once

#include <stdio.h>
#include <mutex>

#ifdef WV_PLATFORM_WINDOWS
#include <Windows.h>
#undef min // these need to be undefined to keep ::min and ::max functions from not dying...
#undef max 
#endif

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	namespace Debug
	{

		namespace Internal
		{

	///////////////////////////////////////////////////////////////////////////////////////

			static const char* LEVEL_STR[] = {
				"          ", // info
				"DEBUG",
				" WARN",
				"ERROR",
				"FATAL",
				"TRACE"
			};

		#ifdef WV_PLATFORM_WINDOWS
			static int LEVEL_COL[] = {
				7,   // info
				11,  // debug
				14,  // warning
				4,   // error
				12,  // fatal
				13   // trace
			};
			
			static HANDLE hConsole = nullptr;
		#endif

			std::mutex& getMutex();

	///////////////////////////////////////////////////////////////////////////////////////

			static inline void SetPrintLevelColor( int _level )
			{
			#ifdef WV_PLATFORM_WINDOWS
				if( !hConsole )
					hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

				if( _level < 0 )
					_level = 0; // default print

				SetConsoleTextAttribute( hConsole, LEVEL_COL[ _level ] );
			#endif
			}
		}

	///////////////////////////////////////////////////////////////////////////////////////

		enum PrintLevel
		{
			WV_PRINT_INFO,
			WV_PRINT_DEBUG,
			WV_PRINT_WARN,
			WV_PRINT_ERROR,
			WV_PRINT_FATAL,
			WV_PRINT_TRACE
		};

	///////////////////////////////////////////////////////////////////////////////////////

		template<typename... Args>
		inline void Print( const char* _str, Args... _args )
		{
		#ifdef WV_PLATFORM_WINDOWS
			Internal::getMutex().lock();
		#endif
			printf( "%s", Internal::LEVEL_STR[ 0 ] );
			printf( _str, _args... );

		#ifdef WV_PLATFORM_WINDOWS
			Internal::getMutex().unlock();
		#endif
		}

		template<typename... Args>
		inline void Print( PrintLevel _printLevel, const char* _str, Args... _args )
		{
		#ifdef WV_PLATFORM_WINDOWS
			Internal::getMutex().lock();
		#endif

			
			bool skip = false;
		#ifndef WV_DEBUG
			skip |= _printLevel == WV_PRINT_DEBUG;
		#endif

			if( skip )
			{
			#ifdef WV_PLATFORM_WINDOWS
				Internal::getMutex().unlock();
			#endif
				return;
			}

			if( _printLevel != WV_PRINT_INFO )
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

		#ifdef WV_PLATFORM_WINDOWS
			Internal::getMutex().unlock();
		#endif
		}
	}
}


