#include "Wyvern.h"
#include <Wyvern/Log/Log.h>

using namespace WV;

Wyvern::Wyvern()
{
	
}

Wyvern::~Wyvern()
{

}

void Wyvern::init()
{
	LOG_WYVERN_MESSAGE( "Wyvern Engine Loaded!\n\n" );
}

Wyvern& Wyvern::getInstance()
{
	static Wyvern instance;
	return instance;
}

void Wyvern::deinit()
{
	
}
