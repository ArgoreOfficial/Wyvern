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
	wv::assimp::Parser parser;

	DEBUG_MATERIAL = _pMaterialRegistry->loadMaterial( "debug" );
	
	CUBE_MESH   = parser.load( "res/meshes/debug-cube.dae", _pMaterialRegistry );
	if( CUBE_MESH )
		for ( int i = 0; i < CUBE_MESH->primitives.size(); i++ )
			CUBE_MESH->primitives[ i ]->material = DEBUG_MATERIAL;
	
	SPHERE_MESH = parser.load( "res/meshes/debug-sphere.dae", _pMaterialRegistry );
	if( SPHERE_MESH )
		for ( int i = 0; i < SPHERE_MESH->primitives.size(); i++ )
			SPHERE_MESH->primitives[ i ]->material = DEBUG_MATERIAL;
	
}

void wv::Debug::Draw::Internal::deinitDebugDraw( iGraphicsDevice* _pGraphicsDevice )
{
	if( CUBE_MESH )
		_pGraphicsDevice->destroyMesh( &CUBE_MESH );

	if( SPHERE_MESH )
		_pGraphicsDevice->destroyMesh( &SPHERE_MESH );

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
		for ( int i = 0; i < spheres.size(); i++ )
		{
			SPHERE_MESH->transform.position = spheres[ i ].position;
			SPHERE_MESH->transform.scale = wv::cVector3f{ spheres[ i ].radius };
			SPHERE_MESH->transform.update();
			_pGraphicsDevice->draw( SPHERE_MESH );
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
