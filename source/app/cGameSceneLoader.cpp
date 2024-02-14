#include "cGameSceneLoader.h"

#include <wv/Core/cApplication.h>

#include <wv/Scene/cSceneManager.h>

#include <wv/Objects/cSpriteObject.h>
#include <wv/Objects/cMeshObject.h>

void cGameSceneLoader::load( wv::cScene* _scene )
{

	/* load scene here */

	printf( "Loading %s\n", _scene->getName().c_str() );

	//wv::cSpriteObject* sprite = new wv::cSpriteObject( "sprite" );
	//sprite->create( "" ); /* TODO: change, make better pls */


	wv::cMeshObject* model = new wv::cMeshObject( "funnycube" );
	model->create( "res/models/funnycube.dae" );

	//_scene->addObject( sprite );
	_scene->addObject( model );
}
