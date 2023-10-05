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
	LOG_MESSAGE( "bob was at home\n" );
	LOG_WARNING( "bob was hungry\n" );
	LOG_ERROR( "bob set the fire alarm off trying to make hamburgers\n" );
}

void WV::Wyvern::getInstance()
{
}
