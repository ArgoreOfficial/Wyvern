#include "windows_file_system.h"

#ifdef WV_PLATFORM_WINDOWS
#include <Windows.h>
OVERLAPPED ol = { 0 };
#endif

std::string GetLastErrorAsString()
{
#ifdef WV_PLATFORM_WINDOWS
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if( errorMessageID == 0 )
	{
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								  NULL, errorMessageID, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&messageBuffer, 0, NULL );

	 //Copy the error message into a std::string.
	std::string message( messageBuffer, size );

	//Free the Win32's string's buffer.
	LocalFree( messageBuffer );

	return message;
#else
	return "";
#endif
}

wv::FileID wv::cWindowsFileSystem::openFile( const char* _path, const eOpenMode& _mode )
{
#ifdef WV_PLATFORM_WINDOWS
	DWORD desiredAccess = 0;
	DWORD shareMode = 0;

	switch( _mode )
	{
	case WV_OPEN_MODE_READ:
		desiredAccess = GENERIC_READ; 
		shareMode = FILE_SHARE_READ;
		break;
	case WV_OPEN_MODE_READWRITE:
		desiredAccess = GENERIC_READ | GENERIC_WRITE; 
		shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;
	case WV_OPEN_MODE_WRITE:
		desiredAccess = GENERIC_WRITE; 
		shareMode = FILE_SHARE_WRITE;
		break;
	}

	HANDLE hFile = CreateFileA(
		_path, desiredAccess, shareMode,
		NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		0 );

	if( hFile == INVALID_HANDLE_VALUE )
		return FileID::InvalidID;

	return FileID( reinterpret_cast<uint64_t>( hFile ) );
#else
	return FileID::InvalidID;
#endif
}

uint64_t wv::cWindowsFileSystem::getFileSize( FileID& _file )
{
#ifdef WV_PLATFORM_WINDOWS
	HANDLE hFile = reinterpret_cast<HANDLE>( _file.value );

	LARGE_INTEGER size{};
	GetFileSizeEx( hFile, &size );

	return size.QuadPart;
#else
	return 0;
#endif
}

int wv::cWindowsFileSystem::readFile( FileID& _file, uint8_t* _buffer, const size_t& _size )
{
#ifdef WV_PLATFORM_WINDOWS
	HANDLE hFile = reinterpret_cast<HANDLE>( _file.value );

	unsigned long bytesRead = 0;
	(void)ReadFile( hFile, _buffer, _size, &bytesRead, &ol );

	return bytesRead;
#else
	return 0;
#endif
}

void wv::cWindowsFileSystem::writeFile( FileID& _file, uint8_t* _buffer, const size_t& _size )
{
#ifdef WV_PLATFORM_WINDOWS
	HANDLE hFile = reinterpret_cast<HANDLE>( _file.value );
#endif
}

void wv::cWindowsFileSystem::closeFile( FileID& _file )
{
#ifdef WV_PLATFORM_WINDOWS
	HANDLE hFile = reinterpret_cast<HANDLE>( _file.value );
	CloseHandle( hFile );

	_file.value = FileID::InvalidID;
#endif
}

