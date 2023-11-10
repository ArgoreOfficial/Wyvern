#pragma once
#include <stdio.h>
// std studio -david 2023

enum eLogLevel
{
	kFATAL = 0,
	kERROR = 1,
	kWARNING = 2,
	kINFO = 3,
	kDEBUG = 4,
	kTRACE = 5
};

void outputLog( eLogLevel _level, const char* message, ... );

#define LOG_RED     printf( "\x1b[31m" )
#define LOG_GREEN   printf( "\x1b[32m" )
#define LOG_YELLOW  printf( "\x1b[33m" )
#define LOG_BLUE    printf( "\x1b[34m" )
#define LOG_MAGENTA printf( "\x1b[35m" )
#define LOG_CYAN    printf( "\x1b[36m" )
#define LOG_RESET   printf( "\x1b[0m"  )

#define WV_FATAL(   msg, ... ) LOG_RED;     outputLog( eLogLevel::kFATAL,   msg, __VA_ARGS__ ); LOG_RESET;
#define WV_ERROR(   msg, ... ) LOG_RED;     outputLog( eLogLevel::kERROR,   msg, __VA_ARGS__ ); LOG_RESET;
#define WV_WARNING( msg, ... ) LOG_YELLOW;  outputLog( eLogLevel::kWARNING, msg, __VA_ARGS__ ); LOG_RESET;
#define WV_INFO(    msg, ... ) LOG_RESET;   outputLog( eLogLevel::kINFO,    msg, __VA_ARGS__ ); LOG_RESET;
#define WV_DEBUG(   msg, ... ) LOG_RESET;   outputLog( eLogLevel::kDEBUG,   msg, __VA_ARGS__ ); LOG_RESET;
