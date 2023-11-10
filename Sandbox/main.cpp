#include "GameLayer.h"

int main()
{
	WV::cApplication::pushLayer( new GameLayer() );
	WV::cApplication::run();

	return 0;
}
