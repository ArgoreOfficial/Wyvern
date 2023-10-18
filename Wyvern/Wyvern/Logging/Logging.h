#pragma once
#include <stdio.h>
// std studio -david 2023

enum logLevel
{
	FATAL = 0,
	ERROR = 1,
	WARNING = 2,
	INFO = 3,
	DEBUG = 4,
	TRACE = 5
};

void outputLog( logLevel _level, const char* message, ... );

#define LOG_RED     printf("\x1b[31m")
#define LOG_GREEN   printf("\x1b[32m")
#define LOG_YELLOW  printf("\x1b[33m")
#define LOG_BLUE    printf("\x1b[34m")
#define LOG_MAGENTA printf("\x1b[35m")
#define LOG_CYAN    printf("\x1b[36m")
#define LOG_RESET   printf("\x1b[0m")

#define WVFATAL(msg, ...)	LOG_RED;    outputLog(logLevel::FATAL, msg, __VA_ARGS__);   LOG_RESET;
#define WVERROR(msg, ...)	LOG_RED;    outputLog(logLevel::ERROR, msg, __VA_ARGS__);   LOG_RESET;
#define WVWARNING(msg, ...) LOG_YELLOW; outputLog(logLevel::WARNING, msg, __VA_ARGS__); LOG_RESET;
#define WVINFO(msg, ...)	LOG_RESET;  outputLog(logLevel::INFO, msg, __VA_ARGS__);    LOG_RESET;
#define WVDEBUG(msg, ...)	LOG_CYAN;   outputLog(logLevel::DEBUG, msg, __VA_ARGS__);   LOG_RESET;
#define WVTRACE(msg, ...)	LOG_MAGENTA;   outputLog(logLevel::TRACE, msg, __VA_ARGS__);   LOG_RESET;
