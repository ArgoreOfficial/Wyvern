#include "cGameSceneLoader.h"

#include <wv/Core/cApplication.h>

#include <wv/Scene/cSceneManager.h>

#include <wv/Objects/cSpriteObject.h>
#include <wv/Objects/cMeshObject.h>

void cGameSceneLoader::load( wv::cScene* _scene )
{

	/* load scene here */

	printf( "Loading %s\n", _scene->getName().c_str() );

	/* TODO: change, make better pls */
	(new wv::cMeshObject( "funnycube" ))->create( "res/models/Glimmer.dae" );
	// (new wv::cSpriteObject( "sprite"    ))->create( "res/materials/sprite" );

}
