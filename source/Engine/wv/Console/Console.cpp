#include "Console.h"

bool wv::Console::Internal::g_initialized = false;

bool wv::Console::isInitialized()
{
	return Internal::g_initialized;
}
