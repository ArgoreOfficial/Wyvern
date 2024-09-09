#include "Draw.h"

#include <wv/Device/GraphicsDevice.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Assets/Materials/MaterialRegistry.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Memory/ModelParser.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::Internal::initDebugDraw( iGraphicsDevice* _pGraphicsDevice, cMaterialRegistry* _pMaterialRegistry )
{
#ifdef WV_PLATFORM_PSVITA
	return;
#endif
	wv::Parser parser;

	DEBUG_MATERIAL = _pMaterialRegistry->loadMaterial( "debug" );

	CUBE_MESH = parser.load( "res/meshes/cube", _pMaterialRegistry );
	if( CUBE_MESH )
		for( int i = 0; i < CUBE_MESH->children[ 0 ]->meshes[ 0 ]->primitives.size(); i++ )
			CUBE_MESH->children[ 0 ]->meshes[ 0 ]->primitives[ i ]->material = DEBUG_MATERIAL;

	SPHERE_MESH = parser.load( "res/meshes/cube", _pMaterialRegistry );
	if( SPHERE_MESH )
		for( int i = 0; i < SPHERE_MESH->children[ 0 ]->meshes[ 0 ]->primitives.size(); i++ )
			SPHERE_MESH->children[ 0 ]->meshes[ 0 ]->primitives[ i ]->material = DEBUG_MATERIAL;

}

void wv::Debug::Draw::Internal::deinitDebugDraw( iGraphicsDevice* _pGraphicsDevice )
{
	if( CUBE_MESH )
		_pGraphicsDevice->destroyMesh( &CUBE_MESH->children[ 0 ]->meshes[ 0 ] );

	if( SPHERE_MESH )
		_pGraphicsDevice->destroyMesh( &SPHERE_MESH->children[ 0 ]->meshes[ 0 ] );

	if( DEBUG_MATERIAL )
	{
		DEBUG_MATERIAL->destroy( _pGraphicsDevice );
		delete DEBUG_MATERIAL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::Internal::drawDebug( iGraphicsDevice* _pGraphicsDevice )
{
	if( SPHERE_MESH )
	{
		for( int i = 0; i < spheres.size(); i++ )
		{
			SPHERE_MESH->transform.position = spheres[ i ].position;
			SPHERE_MESH->transform.scale = wv::cVector3f{ spheres[ i ].radius };
			SPHERE_MESH->transform.update();
			_pGraphicsDevice->drawNode( SPHERE_MESH );
		}
	}

	cubes.clear();
	spheres.clear();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::AddSphere( cVector3f _position, float _radius )
{
	Internal::spheres.push_back( { _position, _radius } );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::AddCube( Transformf _transform )
{
	Internal::cubes.push_back( _transform );
}
