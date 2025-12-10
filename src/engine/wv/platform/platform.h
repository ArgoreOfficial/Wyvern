#pragma once

namespace wv {

class IFileSystem;

extern const char* gFileSystemPathPrefix;

namespace Platform {

IFileSystem* createFileSystem();

}

}