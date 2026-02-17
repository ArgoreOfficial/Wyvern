#pragma once

#include <string>

namespace wv {

class IFileSystem;
class DisplayDriver;
class LowLevelInputQueue;

namespace Platform {

bool initialize();

IFileSystem* createFileSystem( const std::string& _mountedName );
DisplayDriver* createDisplayDriver();

void pollEvents( LowLevelInputQueue& _inputEventQueue );

void cleanup();

}

}