/**
 * @file Thread.h
 * @author Argore
 * @brief 
 * @version 0.1
 * @date 2025-02-03
 * 
 * @copyright Copyright Argore (c) 2025
 * 
 */

#pragma once

#include <stdint.h>

#include <thread>

namespace wv {

namespace Thread {

/**
 * @brief Sleep this thread for the given duration of nanoseconds
 * 
 * @param _nanoseconds the amount of nanoseconds to sleep
 */
void sleepFor( uint64_t _nanoseconds );

/**
 * @brief Sleep this thread for the given duration of seconds
 * 
 * @param _seconds the amount of seconds to sleep
 */
static void sleepForSeconds( double _seconds ) {
	wv::Thread::sleepFor( static_cast<uint64_t>( _seconds * 1000000000LL ) );
}

void yield();

}

}
