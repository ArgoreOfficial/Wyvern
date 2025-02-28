#pragma once

#include <wv/console/console.h>

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
			#ifdef WV_WIDE_PRINT
				"          ", // info
			#else
				"> ", // info
			#endif
				"DEBUG",
				" WARN",
				"ERROR",
				"FATAL",
				"TRACE"
			};

			static const wv::Console::Color LEVEL_COL[] = {
				Console::White,           // info
				Console::Bright_Cyan,     // debug
				Console::Bright_Yellow,   // warning
				Console::Red,             // error
				Console::Bright_Red,      // fatal
				Console::Bright_Magenta   // trace
			};

			std::mutex& getMutex();

	///////////////////////////////////////////////////////////////////////////////////////

			static inline void SetPrintLevelColor( int _level )
			{
				if( _level >= 0 )
					wv::Console::setForegroundColor( LEVEL_COL[ _level ] );
				else
					wv::Console::setForegroundColor( Console::White );

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
			if( !wv::Console::isInitialized() )
				wv::Console::initialize();

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
		inline void logLevelColorStr( const char _levelStr[ 6 ], wv::Console::Color _color, const char* _str, Args... _args )
		{
			printf( "[ " );
			wv::Console::setForegroundColor( _color );
			printf( _levelStr );
			wv::Console::setForegroundColor( Console::White );
			printf( " ] " );
			
			printf( _str, _args... );

		}

		template<typename... Args>
		inline void Print( PrintLevel _printLevel, const char* _str, Args... _args )
		{
			if( !wv::Console::isInitialized() )
				wv::Console::initialize();

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

			if ( _printLevel != WV_PRINT_INFO )
				Debug::logLevelColorStr( 
					Internal::LEVEL_STR[ _printLevel ], 
					Internal::LEVEL_COL[ _printLevel ], 
					_str, _args...);
			else
			{
				printf( Internal::LEVEL_STR[ _printLevel ] );
				printf( _str, _args... );
			}

			// logColorArr( { White, Red, White }, "%s%s%s %s", "[", "DEBUG", "]", "Some other message" );

		#ifdef WV_PLATFORM_WINDOWS
			Internal::getMutex().unlock();
		#endif
		}
	}
}


#define WV_WARNING( ... ) wv::Debug::Print( wv::Debug::WV_PRINT_WARN, __VA_ARGS__ )