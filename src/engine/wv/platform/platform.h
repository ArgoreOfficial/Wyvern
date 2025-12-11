#pragma once

#include <string>

namespace wv {

class IFileSystem;

namespace Platform {

IFileSystem* createFileSystem( const std::string& _mountedName );

}

}