#include "cGameSceneLoader.h"

#include <wv/Core/cApplication.h>

#include <wv/Scene/cSceneManager.h>

#include <wv/Objects/cSpriteObject.h>
#include <wv/Objects/cMeshObject.h>

void cGameSceneLoader::load( wv::cScene* _scene )
{

	/* load scene here */

	printf( "Loading %s\n", _scene->getName().c_str() );
	/*
	for ( int i = 0; i < 500; i++ )
	{
		// wv::cMeshObject* m = ( new wv::cMeshObject( "funnycube" ) )->create( "res/models/Glimmer.dae" );
		// delete m;
	}
	*/

	/* TODO: change, make better pls */
	
	_scene->addObject( ( new wv::cMeshObject( "City17" ) )->create( "res/models/City17/c17-sep.dae" ) );
	//_scene->addObject( ( new wv::cMeshObject( "funnycube" ) )->create( "res/models/Glimmer.dae" ) );
	
	// (new wv::cSpriteObject( "sprite"    ))->create( "res/materials/sprite" );
}

