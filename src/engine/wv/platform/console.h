#pragma once

#include <stdint.h>

namespace wv {

namespace Console {

namespace Internal {

extern bool g_initialized;

const uint8_t ANSI_EscapeCodes[] = {
	30, // Black,
	31, // Red,
	32, // Green,
	33, // Yellow,
	34, // Blue,
	35, // Magenta,
	36, // Cyan,
	37, // White,
	90, // Gray,
	91, // Bright_Red,
	92, // Bright_Green,
	93, // Bright_Yellow,
	94, // Bright_Blue,
	95, // Bright_Magenta,
	96, // Bright_Cyan,
	97  // Bright_White
};

#define WV_ANSI_ESC_STR( _v ) "\x1b["#_v"m"
//\x1b[31m
const char* const ANSI_EscapeStrsFG[] = {
	WV_ANSI_ESC_STR( 30 ),
	WV_ANSI_ESC_STR( 31 ),
	WV_ANSI_ESC_STR( 32 ),
	WV_ANSI_ESC_STR( 33 ),
	WV_ANSI_ESC_STR( 34 ),
	WV_ANSI_ESC_STR( 35 ),
	WV_ANSI_ESC_STR( 36 ),
	WV_ANSI_ESC_STR( 37 ),
	WV_ANSI_ESC_STR( 90 ),
	WV_ANSI_ESC_STR( 91 ),
	WV_ANSI_ESC_STR( 92 ),
	WV_ANSI_ESC_STR( 93 ),
	WV_ANSI_ESC_STR( 94 ),
	WV_ANSI_ESC_STR( 95 ),
	WV_ANSI_ESC_STR( 96 ),
	WV_ANSI_ESC_STR( 97 )
};

// BG = FG+10

}

// https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
enum Color : uint8_t
{
	Black          =  0,
	Red            =  1,
	Green          =  2,
	Yellow         =  3,
	Blue           =  4,
	Magenta        =  5,
	Cyan           =  6,
	White          =  7,
	Gray           =  8,
	Bright_Red     =  9,
	Bright_Green   = 10,
	Bright_Yellow  = 11,
	Bright_Blue    = 12,
	Bright_Magenta = 13,
	Bright_Cyan    = 14,
	Bright_White   = 15
};

bool isInitialized();
void initialize();

void setForegroundColor( Color _color );
void setBackgroundColor( Color _color );

}

}
