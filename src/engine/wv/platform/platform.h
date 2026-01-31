#pragma once

#include <string>

namespace wv {

class IFileSystem;
class DisplayDriver;

namespace Platform {

bool initialize();

IFileSystem* createFileSystem( const std::string& _mountedName );
DisplayDriver* createDisplayDriver();

void pollEvents();

void cleanup();

}

}