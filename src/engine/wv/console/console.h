#pragma once

#include <stdint.h>
#include <string>
#include <vector>

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
	Color_Black         =  0,
	Color_Red           =  1,
	Color_Green         =  2,
	Color_Yellow        =  3,
	Color_Blue          =  4,
	Color_Magenta       =  5,
	Color_Cyan          =  6,
	Color_White         =  7,
	Color_Gray          =  8,
	Color_BrightRed     =  9,
	Color_BrightGreen   = 10,
	Color_BrightYellow  = 11,
	Color_BrightBlue    = 12,
	Color_BrightMagenta = 13,
	Color_BrightCyan    = 14,
	Color_BrightWhite   = 15
};

bool isInitialized();
void initialize();
void deinitialize();

void setForegroundColor( Color _color );
//void setBackgroundColor( Color _color );

int run( const std::string& _runDir, const std::vector<std::string>& _cmd );
static int run( const std::vector<std::string>& _cmd ) {
	return run( "./", _cmd );
}

}

}
