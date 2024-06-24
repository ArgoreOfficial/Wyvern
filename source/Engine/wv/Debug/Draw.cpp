#include "Draw.h"

#include <wv/Device/GraphicsDevice.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Memory/ModelParser.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::Internal::initDebugDraw( iGraphicsDevice* _device )
{
	wv::assimp::Parser parser;
	DEBUG_MATERIAL = new Material();
	DEBUG_MATERIAL->loadFromFile( "debug" );

	CUBE_MESH   = parser.load( "res/meshes/debug-cube.dae" );
	for ( int i = 0; i < CUBE_MESH->primitives.size(); i++ )
		CUBE_MESH->primitives[ i ]->material = DEBUG_MATERIAL;
	
	SPHERE_MESH = parser.load( "res/meshes/debug-sphere.dae" );
	for ( int i = 0; i < SPHERE_MESH->primitives.size(); i++ )
		SPHERE_MESH->primitives[ i ]->material = DEBUG_MATERIAL;
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::Internal::drawDebug( iGraphicsDevice* _device )
{
	for ( int i = 0; i < spheres.size(); i++ )
	{
		SPHERE_MESH->transform.position = spheres[ i ].position;
		SPHERE_MESH->transform.scale = wv::Vector3f{ spheres[ i ].radius };
		_device->draw( SPHERE_MESH );
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
