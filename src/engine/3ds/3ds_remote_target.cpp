#include "3ds_remote_target.h"

#include <wv/debug/log.h>

#ifdef WV_PLATFORM_3DS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <3ds.h>

bool wv::Remote::isRunningRemoteTarget( int argc, char* argv[] )
{
	return strncmp( argv[ 0 ], "RMT", 3 ) == 0;
}

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32* SOC_buffer = NULL;
s32 sock = -1, csock = -1;

__attribute__( ( format( printf, 1, 2 ) ) )
void failExit( const char* fmt, ... );

void socShutdown() {
	printf( "waiting for socExit...\n" );
	socExit();
}

void wv::Remote::remoteMain()
{
	atexit( gfxExit );

	SOC_buffer = (u32*)memalign( SOC_ALIGN, SOC_BUFFERSIZE );

	if( SOC_buffer == NULL )
		failExit( "memalign: failed to allocate\n" );
	
	int ret;
	if( ( ret = socInit( SOC_buffer, SOC_BUFFERSIZE ) ) != 0 )
		failExit( "socInit: 0x%08X\n", (unsigned int)ret );
	
	atexit( socShutdown );
	link3dsStdio();

}

void wv::Remote::remoteMainExit()
{
	close( sock );
}

void failExit( const char* fmt, ... ) {
	if( sock > 0 ) close( sock );
	if( csock > 0 ) close( csock );

	va_list ap;

	printf( CONSOLE_RED );
	va_start( ap, fmt );
	vprintf( fmt, ap );
	va_end( ap );
	printf( CONSOLE_RESET );
	printf( "\nPress B to exit\n" );

	while( aptMainLoop() )
	{
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if( kDown & KEY_B ) exit( 0 );
	}
}
#endif

int wv::RemoteTarget3DS::buildExecutable( const std::string& _mode )
{
	const std::vector<std::string> cmd = {
		"xmake",
		"f -c",
		"-a arm_3ds",
		"-p 3DS",
		"-m", _mode
	};
	int err = wv::Console::run( "../../", cmd );

	if ( err )
		return err;

	return wv::Console::run( { "xmake" } );;
}

int wv::RemoteTarget3DS::remoteLaunchExecutable( const std::string& _name, const std::vector<std::string>& _args )
{
	int pingErr = wv::Console::run( {
		"ping",
		"-n 1",
		m_address,
		"> nul"
	} );

	if( pingErr )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Failed to ping 3DS '%s'\n", m_address );
		return pingErr;
	}

	int err = wv::Console::run( { 
		"../../tools/3ds/3dslink",
		"-a", m_address,
		"-0 \"RMT\"", // argv[0] == "RMT" -> launches remote client
		"-s",
		"../3ds/" + _name + ".3dsx" 
	} );

	return err;
}
