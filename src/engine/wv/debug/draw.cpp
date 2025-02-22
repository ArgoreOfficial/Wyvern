#include "draw.h"

#include <wv/graphics/graphics_device.h>
#include <wv/material/material.h>
#include <wv/mesh/mesh_resource.h>
#include <wv/memory/model_parser.h>

#include <wv/resource/resource_registry.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::MeshNode* wv::Debug::Draw::Internal::SPHERE_MESH = nullptr;
wv::MeshNode* wv::Debug::Draw::Internal::CUBE_MESH   = nullptr;

std::vector<wv::Debug::Draw::Internal::Sphere> wv::Debug::Draw::Internal::spheres{};
std::vector<wv::Transformf> wv::Debug::Draw::Internal::cubes{};

void wv::Debug::Draw::Internal::initDebugDraw( IGraphicsDevice* _pLowLevelGraphics, ResourceRegistry* _pResourceRegistry )
{
#ifdef WV_PLATFORM_PSVITA
	return;
#endif
	wv::Parser parser;

	CUBE_MESH = parser.load( "meshes/DebugCube.dae", _pResourceRegistry );
	SPHERE_MESH = parser.load( "meshes/DebugCube.dae", _pResourceRegistry );

}

void wv::Debug::Draw::Internal::deinitDebugDraw( IGraphicsDevice* _pLowLevelGraphics )
{
	if( CUBE_MESH )
	{
		_pLowLevelGraphics->destroyMesh( CUBE_MESH->children[ 0 ]->meshes[ 0 ] );
		WV_FREE( CUBE_MESH->children[ 0 ] );
		WV_FREE( CUBE_MESH );
	}
	
	if( SPHERE_MESH )
	{
		_pLowLevelGraphics->destroyMesh( SPHERE_MESH->children[ 0 ]->meshes[ 0 ] );
		WV_FREE( SPHERE_MESH->children[ 0 ] );
		WV_FREE( SPHERE_MESH );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Debug::Draw::Internal::drawDebug( IGraphicsDevice* _pLowLevelGraphics )
{
	if( SPHERE_MESH )
	{
		for( size_t i = 0; i < spheres.size(); i++ )
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
