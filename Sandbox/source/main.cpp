#include <Wyvern.h>

#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
	SetConsoleTitleA( "Wyvern Log" );
#endif
	WV::Wyvern::init();

	return 0;
}