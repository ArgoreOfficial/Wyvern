#pragma once

#include <stdint.h>
#include <Windows.h>

namespace arx {

static uintptr_t getBaseAddr() {
	static uintptr_t g_baseAddr = reinterpret_cast<uintptr_t>( GetModuleHandleA( NULL ) );
	return g_baseAddr;
}

}