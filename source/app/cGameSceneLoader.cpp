#include "cGameSceneLoader.h"

#include <wv/Core/cApplication.h>

#include <wv/Scene/cSceneManager.h>

#include <wv/Objects/cSpriteObject.h>

void cGameSceneLoader::load( wv::cScene* _scene )
{

	/* load scene here */

	printf( "Loading %s\n", _scene->getName().c_str() );

	wv::cSpriteObject* sprite = new wv::cSpriteObject( "sprite" );
	sprite->create( "../res/textures/funnyman.png" );

	_scene->addObject( sprite );

}
