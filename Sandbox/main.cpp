#include "GameLayer.h"

int main()
{
	WV::Application::pushLayer( new GameLayer() );
	WV::Application::run();

	return 0;
}
