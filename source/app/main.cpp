#include <stdio.h>

#include <wv/Core/cApplication.h>
#include "cGameSceneLoader.h"

int main()
{
	wv::cApplication::getInstance().create();
	wv::cApplication::getInstance().run( new cGameSceneLoader() );
}