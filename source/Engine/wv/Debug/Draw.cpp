#include "Draw.h"

#include <wv/Graphics/Graphics.h>
#include <wv/Material/Material.h>
#include <wv/Mesh/MeshResource.h>
#include <wv/Memory/ModelParser.h>

#include <wv/Resource/ResourceRegistry.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::Internal::initDebugDraw( iLowLevelGraphics* _pLowLevelGraphics, cResourceRegistry* _pResourceRegistry )
{
#ifdef WV_PLATFORM_PSVITA
	return;
#endif
	wv::Parser parser;

	CUBE_MESH = parser.load( "res/meshes/DebugCube.dae", _pResourceRegistry );
	SPHERE_MESH = parser.load( "res/meshes/DebugCube.dae", _pResourceRegistry );

}

void wv::Debug::Draw::Internal::deinitDebugDraw( iLowLevelGraphics* _pLowLevelGraphics )
{
	if( CUBE_MESH )
		_pLowLevelGraphics->_destroyMesh( CUBE_MESH->children[ 0 ]->meshes[ 0 ] );
	
	if( SPHERE_MESH )
		_pLowLevelGraphics->_destroyMesh( SPHERE_MESH->children[ 0 ]->meshes[ 0 ] );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::Internal::drawDebug( iLowLevelGraphics* _pLowLevelGraphics )
{
	if( SPHERE_MESH )
	{
		for( int i = 0; i < spheres.size(); i++ )
		{
			SPHERE_MESH->transform.position = spheres[ i ].position;
			SPHERE_MESH->transform.scale = wv::Vector3f{ spheres[ i ].radius };
			SPHERE_MESH->transform.update( nullptr );
			
			// _pLowLevelGraphics->drawNode( SPHERE_MESH );
		}
	}

	cubes.clear();
	spheres.clear();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::AddSphere( Vector3f _position, float _radius )
{
	Internal::spheres.push_back( { _position, _radius } );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::AddCube( Transformf _transform )
{
	Internal::cubes.push_back( _transform );
}
