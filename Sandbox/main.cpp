#include "GameLayer.h"

int main()
{
	wv::cApplication::pushLayer( new GameLayer() );
	wv::cApplication::run();

	return 0;
}
