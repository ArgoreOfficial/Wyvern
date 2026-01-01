#include "windows_keyboard_driver.h"

#include <wv/debug/log.h>

#include <Windows.h>

// https://handmade.network/forums/articles/t/2823-keyboard_inputs_-_scancodes%252C_raw_input%252C_text_input%252C_key_names

enum WindowsScancode
{
	sc_escape = 0x01,
	sc_1 = 0x02,
	sc_2 = 0x03,
	sc_3 = 0x04,
	sc_4 = 0x05,
	sc_5 = 0x06,
	sc_6 = 0x07,
	sc_7 = 0x08,
	sc_8 = 0x09,
	sc_9 = 0x0A,
	sc_0 = 0x0B,
	sc_minus = 0x0C,
	sc_equals = 0x0D,
	sc_backspace = 0x0E,
	sc_tab = 0x0F,
	sc_q = 0x10,
	sc_w = 0x11,
	sc_e = 0x12,
	sc_r = 0x13,
	sc_t = 0x14,
	sc_y = 0x15,
	sc_u = 0x16,
	sc_i = 0x17,
	sc_o = 0x18,
	sc_p = 0x19,
	sc_bracketLeft = 0x1A,
	sc_bracketRight = 0x1B,
	sc_enter = 0x1C,
	sc_controlLeft = 0x1D,
	sc_a = 0x1E,
	sc_s = 0x1F,
	sc_d = 0x20,
	sc_f = 0x21,
	sc_g = 0x22,
	sc_h = 0x23,
	sc_j = 0x24,
	sc_k = 0x25,
	sc_l = 0x26,
	sc_semicolon = 0x27,
	sc_apostrophe = 0x28,
	sc_grave = 0x29,
	sc_shiftLeft = 0x2A,
	sc_backslash = 0x2B,
	sc_z = 0x2C,
	sc_x = 0x2D,
	sc_c = 0x2E,
	sc_v = 0x2F,
	sc_b = 0x30,
	sc_n = 0x31,
	sc_m = 0x32,
	sc_comma = 0x33,
	sc_period = 0x34,
	sc_slash = 0x35,
	sc_shiftRight = 0x36,
	sc_numpad_multiply = 0x37,
	sc_altLeft = 0x38,
	sc_space = 0x39,
	sc_capsLock = 0x3A,
	sc_f1 = 0x3B,
	sc_f2 = 0x3C,
	sc_f3 = 0x3D,
	sc_f4 = 0x3E,
	sc_f5 = 0x3F,
	sc_f6 = 0x40,
	sc_f7 = 0x41,
	sc_f8 = 0x42,
	sc_f9 = 0x43,
	sc_f10 = 0x44,
	sc_numLock = 0x45,
	sc_scrollLock = 0x46,
	sc_numpad_7 = 0x47,
	sc_numpad_8 = 0x48,
	sc_numpad_9 = 0x49,
	sc_numpad_minus = 0x4A,
	sc_numpad_4 = 0x4B,
	sc_numpad_5 = 0x4C,
	sc_numpad_6 = 0x4D,
	sc_numpad_plus = 0x4E,
	sc_numpad_1 = 0x4F,
	sc_numpad_2 = 0x50,
	sc_numpad_3 = 0x51,
	sc_numpad_0 = 0x52,
	sc_numpad_period = 0x53,
	sc_alt_printScreen = 0x54, /* Alt + print screen. MapVirtualKeyEx( VK_SNAPSHOT, MAPVK_VK_TO_VSC_EX, 0 ) returns scancode 0x54. */
	sc_bracketAngle = 0x56, /* Key between the left shift and Z. */
	sc_f11 = 0x57,
	sc_f12 = 0x58,
	sc_oem_1 = 0x5a, /* VK_OEM_WSCTRL */
	sc_oem_2 = 0x5b, /* VK_OEM_FINISH */
	sc_oem_3 = 0x5c, /* VK_OEM_JUMP */
	sc_eraseEOF = 0x5d,
	sc_oem_4 = 0x5e, /* VK_OEM_BACKTAB */
	sc_oem_5 = 0x5f, /* VK_OEM_AUTO */
	sc_zoom = 0x62,
	sc_help = 0x63,
	sc_f13 = 0x64,
	sc_f14 = 0x65,
	sc_f15 = 0x66,
	sc_f16 = 0x67,
	sc_f17 = 0x68,
	sc_f18 = 0x69,
	sc_f19 = 0x6a,
	sc_f20 = 0x6b,
	sc_f21 = 0x6c,
	sc_f22 = 0x6d,
	sc_f23 = 0x6e,
	sc_oem_6 = 0x6f, /* VK_OEM_PA3 */
	sc_katakana = 0x70,
	sc_oem_7 = 0x71, /* VK_OEM_RESET */
	sc_f24 = 0x76,
	sc_sbcschar = 0x77,
	sc_convert = 0x79,
	sc_nonconvert = 0x7B, /* VK_OEM_PA1 */

	sc_media_previous = 0xE010,
	sc_media_next = 0xE019,
	sc_numpad_enter = 0xE01C,
	sc_controlRight = 0xE01D,
	sc_volume_mute = 0xE020,
	sc_launch_app2 = 0xE021,
	sc_media_play = 0xE022,
	sc_media_stop = 0xE024,
	sc_volume_down = 0xE02E,
	sc_volume_up = 0xE030,
	sc_browser_home = 0xE032,
	sc_numpad_divide = 0xE035,
	sc_printScreen = 0xE037,
	
	sc_altRight = 0xE038,
	sc_cancel = 0xE046, /* CTRL + Pause */
	sc_home = 0xE047,
	sc_arrowUp = 0xE048,
	sc_pageUp = 0xE049,
	sc_arrowLeft = 0xE04B,
	sc_arrowRight = 0xE04D,
	sc_end = 0xE04F,
	sc_arrowDown = 0xE050,
	sc_pageDown = 0xE051,
	sc_insert = 0xE052,
	sc_delete = 0xE053,
	sc_metaLeft = 0xE05B,
	sc_metaRight = 0xE05C,
	sc_application = 0xE05D,
	sc_power = 0xE05E,
	sc_sleep = 0xE05F,
	sc_wake = 0xE063,
	sc_browser_search = 0xE065,
	sc_browser_favorites = 0xE066,
	sc_browser_refresh = 0xE067,
	sc_browser_stop = 0xE068,
	sc_browser_forward = 0xE069,
	sc_browser_back = 0xE06A,
	sc_launch_app1 = 0xE06B,
	sc_launch_email = 0xE06C,
	sc_launch_media = 0xE06D,

	sc_pause = 0xE11D45,
	/*
	sc_pause:
	- make: 0xE11D 45 0xE19D C5
	- make in raw input: 0xE11D 0x45
	- break: none
	- No repeat when you hold the key down
	- There are no break so I don't know how the key down/up is expected to work. Raw input sends "keydown" and "keyup" messages, and it appears that the keyup message is sent directly after the keydown message (you can't hold the key down) so depending on when GetMessage or PeekMessage will return messages, you may get both a keydown and keyup message "at the same time". If you use VK messages most of the time you only get keydown messages, but some times you get keyup messages too.
	- when pressed at the same time as one or both control keys, generates a 0xE046 (sc_cancel) and the string for that scancode is "break".
	*/
};

struct ScancodePair
{
	WindowsScancode winsc;
	wv::Scancode wvsc;
};

const ScancodePair scancodePairs[] = {
	{ sc_escape,            wv::SCANCODE_ESCAPE },
	{ sc_1,                 wv::SCANCODE_ALPHA_1 },
	{ sc_2,                 wv::SCANCODE_ALPHA_2 },
	{ sc_3,                 wv::SCANCODE_ALPHA_3 },
	{ sc_4,                 wv::SCANCODE_ALPHA_4 },
	{ sc_5,                 wv::SCANCODE_ALPHA_5 },
	{ sc_6,                 wv::SCANCODE_ALPHA_6 },
	{ sc_7,                 wv::SCANCODE_ALPHA_7 },
	{ sc_8,                 wv::SCANCODE_ALPHA_8 },
	{ sc_9,                 wv::SCANCODE_ALPHA_9 },
	{ sc_0,                 wv::SCANCODE_ALPHA_0 },
	{ sc_minus,             wv::SCANCODE_MINUS },
	{ sc_equals,            wv::SCANCODE_EQUALS },
	{ sc_backspace,         wv::SCANCODE_BACKSPACE },
	{ sc_tab,               wv::SCANCODE_TAB },
	{ sc_q,                 wv::SCANCODE_Q },
	{ sc_w,                 wv::SCANCODE_W },
	{ sc_e,                 wv::SCANCODE_E },
	{ sc_r,                 wv::SCANCODE_R },
	{ sc_t,                 wv::SCANCODE_T },
	{ sc_y,                 wv::SCANCODE_Y },
	{ sc_u,                 wv::SCANCODE_U },
	{ sc_i,                 wv::SCANCODE_I },
	{ sc_o,                 wv::SCANCODE_O },
	{ sc_p,                 wv::SCANCODE_P },
	{ sc_bracketLeft,       wv::SCANCODE_LEFT_BRACKET },
	{ sc_bracketRight,      wv::SCANCODE_RIGHT_BRACKET },
	{ sc_enter,             wv::SCANCODE_RETURN },
	{ sc_controlLeft,       wv::SCANCODE_NONE },
	{ sc_a,                 wv::SCANCODE_A },
	{ sc_s,                 wv::SCANCODE_S },
	{ sc_d,                 wv::SCANCODE_D },
	{ sc_f,                 wv::SCANCODE_F },
	{ sc_g,                 wv::SCANCODE_G },
	{ sc_h,                 wv::SCANCODE_H },
	{ sc_j,                 wv::SCANCODE_J },
	{ sc_k,                 wv::SCANCODE_K },
	{ sc_l,                 wv::SCANCODE_L },
	{ sc_semicolon,         wv::SCANCODE_SEMICOLON },
	{ sc_apostrophe,        wv::SCANCODE_APOSTROPHE },
	{ sc_grave,             wv::SCANCODE_GRAVE },
	{ sc_shiftLeft,         wv::SCANCODE_NONE },
	{ sc_backslash,         wv::SCANCODE_BACKSLASH },
	{ sc_z,                 wv::SCANCODE_Z },
	{ sc_x,                 wv::SCANCODE_X },
	{ sc_c,                 wv::SCANCODE_C },
	{ sc_v,                 wv::SCANCODE_V },
	{ sc_b,                 wv::SCANCODE_B },
	{ sc_n,                 wv::SCANCODE_N },
	{ sc_m,                 wv::SCANCODE_M },
	{ sc_comma,             wv::SCANCODE_COMMA },
	{ sc_period,            wv::SCANCODE_PERIOD },
	{ sc_slash,             wv::SCANCODE_SLASH },
	{ sc_shiftRight,        wv::SCANCODE_NONE },
	{ sc_numpad_multiply,   wv::SCANCODE_NONE },
	{ sc_altLeft,           wv::SCANCODE_NONE },
	{ sc_space,             wv::SCANCODE_SPACE },
	{ sc_capsLock,          wv::SCANCODE_CAPSLOCK },
	{ sc_f1,                wv::SCANCODE_NONE },
	{ sc_f2,                wv::SCANCODE_NONE },
	{ sc_f3,                wv::SCANCODE_NONE },
	{ sc_f4,                wv::SCANCODE_NONE },
	{ sc_f5,                wv::SCANCODE_NONE },
	{ sc_f6,                wv::SCANCODE_NONE },
	{ sc_f7,                wv::SCANCODE_NONE },
	{ sc_f8,                wv::SCANCODE_NONE },
	{ sc_f9,                wv::SCANCODE_NONE },
	{ sc_f10,               wv::SCANCODE_NONE },
	{ sc_numLock,           wv::SCANCODE_NONE },
	{ sc_scrollLock,        wv::SCANCODE_NONE },
	{ sc_numpad_7,          wv::SCANCODE_NONE },
	{ sc_numpad_8,          wv::SCANCODE_NONE },
	{ sc_numpad_9,          wv::SCANCODE_NONE },
	{ sc_numpad_minus,      wv::SCANCODE_NONE },
	{ sc_numpad_4,          wv::SCANCODE_NONE },
	{ sc_numpad_5,          wv::SCANCODE_NONE },
	{ sc_numpad_6,          wv::SCANCODE_NONE },
	{ sc_numpad_plus,       wv::SCANCODE_NONE },
	{ sc_numpad_1,          wv::SCANCODE_NONE },
	{ sc_numpad_2,          wv::SCANCODE_NONE },
	{ sc_numpad_3,          wv::SCANCODE_NONE },
	{ sc_numpad_0,          wv::SCANCODE_NONE },
	{ sc_numpad_period,     wv::SCANCODE_NONE },
	{ sc_alt_printScreen,   wv::SCANCODE_NONE },
	{ sc_bracketAngle,      wv::SCANCODE_NONE },
	{ sc_f11,               wv::SCANCODE_NONE },
	{ sc_f12,               wv::SCANCODE_NONE },
	{ sc_oem_1,             wv::SCANCODE_NONE },
	{ sc_oem_2,             wv::SCANCODE_NONE },
	{ sc_oem_3,             wv::SCANCODE_NONE },
	{ sc_eraseEOF,          wv::SCANCODE_NONE },
	{ sc_oem_4,             wv::SCANCODE_NONE },
	{ sc_oem_5,             wv::SCANCODE_NONE },
	{ sc_zoom,              wv::SCANCODE_NONE },
	{ sc_help,              wv::SCANCODE_NONE },
	{ sc_f13,               wv::SCANCODE_NONE },
	{ sc_f14,               wv::SCANCODE_NONE },
	{ sc_f15,               wv::SCANCODE_NONE },
	{ sc_f16,               wv::SCANCODE_NONE },
	{ sc_f17,               wv::SCANCODE_NONE },
	{ sc_f18,               wv::SCANCODE_NONE },
	{ sc_f19,               wv::SCANCODE_NONE },
	{ sc_f20,               wv::SCANCODE_NONE },
	{ sc_f21,               wv::SCANCODE_NONE },
	{ sc_f22,               wv::SCANCODE_NONE },
	{ sc_f23,               wv::SCANCODE_NONE },
	{ sc_oem_6,             wv::SCANCODE_NONE },
	{ sc_katakana,          wv::SCANCODE_NONE },
	{ sc_oem_7,             wv::SCANCODE_NONE },
	{ sc_f24,               wv::SCANCODE_NONE },
	{ sc_sbcschar,          wv::SCANCODE_NONE },
	{ sc_convert,           wv::SCANCODE_NONE },
	{ sc_nonconvert,        wv::SCANCODE_NONE },
	{ sc_media_previous,    wv::SCANCODE_NONE },
	{ sc_media_next,        wv::SCANCODE_NONE },
	{ sc_numpad_enter,      wv::SCANCODE_NONE },
	{ sc_controlRight,      wv::SCANCODE_NONE },
	{ sc_volume_mute,       wv::SCANCODE_NONE },
	{ sc_launch_app2,       wv::SCANCODE_NONE },
	{ sc_media_play,        wv::SCANCODE_NONE },
	{ sc_media_stop,        wv::SCANCODE_NONE },
	{ sc_volume_down,       wv::SCANCODE_NONE },
	{ sc_volume_up,         wv::SCANCODE_NONE },
	{ sc_browser_home,      wv::SCANCODE_NONE },
	{ sc_numpad_divide,     wv::SCANCODE_NONE },
	{ sc_printScreen,       wv::SCANCODE_NONE },
	{ sc_altRight,          wv::SCANCODE_NONE },
	{ sc_cancel,            wv::SCANCODE_NONE },
	{ sc_home,              wv::SCANCODE_NONE },
	{ sc_arrowUp,           wv::SCANCODE_ARROW_UP },
	{ sc_pageUp,            wv::SCANCODE_NONE },
	{ sc_arrowLeft,         wv::SCANCODE_ARROW_LEFT },
	{ sc_arrowRight,        wv::SCANCODE_ARROW_RIGHT },
	{ sc_end,               wv::SCANCODE_NONE },
	{ sc_arrowDown,         wv::SCANCODE_ARROW_DOWN },
	{ sc_pageDown,          wv::SCANCODE_NONE },
	{ sc_insert,            wv::SCANCODE_NONE },
	{ sc_delete,            wv::SCANCODE_NONE },
	{ sc_metaLeft,          wv::SCANCODE_NONE },
	{ sc_metaRight,         wv::SCANCODE_NONE },
	{ sc_application,       wv::SCANCODE_NONE },
	{ sc_power,             wv::SCANCODE_NONE },
	{ sc_sleep,             wv::SCANCODE_NONE },
	{ sc_wake,              wv::SCANCODE_NONE },
	{ sc_browser_search,    wv::SCANCODE_NONE },
	{ sc_browser_favorites, wv::SCANCODE_NONE },
	{ sc_browser_refresh,   wv::SCANCODE_NONE },
	{ sc_browser_stop,      wv::SCANCODE_NONE },
	{ sc_browser_forward,   wv::SCANCODE_NONE },
	{ sc_browser_back,      wv::SCANCODE_NONE },
	{ sc_launch_app1,       wv::SCANCODE_NONE },
	{ sc_launch_email,      wv::SCANCODE_NONE },
	{ sc_launch_media,      wv::SCANCODE_NONE },
	{ sc_pause,             wv::SCANCODE_NONE }
};

wv::Scancode winToWvScancode( uint32_t _scancode ) {
	for ( size_t i = 0; i < 154; i++ )
		if ( scancodePairs[ i ].winsc == _scancode )
			return scancodePairs[ i ].wvsc;
	return wv::SCANCODE_NONE;
}

UINT wvToWinScancode( wv::Scancode _scancode ) {
	for ( size_t i = 0; i < 154; i++ )
		if ( scancodePairs[ i ].wvsc == _scancode )
			return (UINT)scancodePairs[ i ].winsc;
	return 0;
}

void wv::WindowsKeyboardDriver::updateDriver( InputSystem* _inputSystem )
{
	for ( size_t i = 1; i < SCANCODE_MAX; i++ )
	{
		m_prevScancodeStates[ i ] = m_scancodeStates[ i ];

		UINT scancode = wvToWinScancode( (wv::Scancode)i );
		if ( scancode == 0 )
			continue;

		UINT vkey = MapVirtualKeyA( scancode, MAPVK_VSC_TO_VK );
		if ( vkey == 0 )
			continue;

		SHORT keyinfo = GetKeyState( vkey );
		bool state       = keyinfo & 0x8000;
		bool justPressed = keyinfo & 0x01;

		if ( m_scancodeStates[ i ] != state )
		{
			// This is important
			m_scancodeStates[ i ] = state;
		}
	}

	sendTriggerEvents( _inputSystem );
	sendValueEvents( _inputSystem );
	sendAxisEvents( _inputSystem );
}
