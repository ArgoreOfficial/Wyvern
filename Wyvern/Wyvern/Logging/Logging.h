#pragma once
#include <stdio.h>
/*
#include <gl/glew.h>
#include <GLFW/glfw3.h>
*/


// std studio -david 2023

enum logLevel
{
	FATAL = 0,
	ERROR = 1,
	WARNING = 2,
	INFO = 3,
	DEBUG = 4
};

void outputLog( logLevel _level, const char* message, ... );

#define LOG_RESET printf("\033[0m")
#define LOG_WHITE printf("\033[0;37m")
#define LOG_YELLOW printf("\033[0;33m")
#define LOG_RED printf("\033[0;31m")
#define LOG_CYAN printf("\033[0;36m")

#define WVFATAL(msg, ...) outputLog(logLevel::FATAL, msg, __VA_ARGS__)
#define WVERROR(msg, ...) outputLog(logLevel::ERROR, msg, __VA_ARGS__)
#define WVWARNING(msg, ...) outputLog(logLevel::WARNING, msg, __VA_ARGS__)
#define WVINFO(msg, ...) outputLog(logLevel::INFO, msg, __VA_ARGS__)
#define WVDEBUG(msg, ...) outputLog(logLevel::DEBUG, msg, __VA_ARGS__)
/*
void GLAPIENTRY glLogCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam );
*/