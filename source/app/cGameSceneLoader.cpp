#include "cGameSceneLoader.h"

#include <wv/Core/cApplication.h>

#include <wv/Scene/cSceneManager.h>

#include <wv/Objects/cSpriteObject.h>
#include <wv/Objects/cMeshObject.h>

void cGameSceneLoader::load( wv::cScene* _scene )
{

	_scene->addObject( ( new wv::cMeshObject( "City17" ) )->create( "res/models/City17/city17.dae" ) );
	//_scene->addObject( ( new wv::cMeshObject( "funnycube" ) )->create( "res/models/Glimmer.dae" ) );
	
	// (new wv::cSpriteObject( "sprite"    ))->create( "res/materials/sprite" );
}

