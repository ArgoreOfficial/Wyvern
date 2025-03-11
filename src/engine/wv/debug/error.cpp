#include "error.h"

#include <signal.h>

void wv::Error::init()
{
	signal( SIGSEGV, wv::SignalHandler );
}
