#include "Print.h"

std::mutex& wv::Debug::Internal::getMutex()
{
	static std::mutex PRINT_MUTEX{};
	return PRINT_MUTEX;
}
