#ifdef NOTDEFINED

#pragma once

#include <wv/math/transform.h>
#include <wv/math/vector3.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct MeshNode;
	class Material;
	class IGraphicsDevice;
	class ResourceRegistry;

///////////////////////////////////////////////////////////////////////////////////////

	namespace Debug 
	{
		namespace Draw
		{

	///////////////////////////////////////////////////////////////////////////////////////

			namespace Internal
			{
				struct Sphere { Vector3f position; float radius; }; /// TODO: move to wv/Math or similar

				extern MeshNode* SPHERE_MESH;
				extern MeshNode* CUBE_MESH;
				
				extern std::vector<Sphere> spheres;
				extern std::vector<Transformf> cubes;

				void initDebugDraw( IGraphicsDevice* _pGraphicsDevice, ResourceRegistry* _pResourceRegistry );
				void deinitDebugDraw( IGraphicsDevice* _pGraphicsDevice );
				void drawDebug( IGraphicsDevice* _pGraphicsDevice );
			}

	///////////////////////////////////////////////////////////////////////////////////////

			void AddSphere( Vector3f _position, float _radius = 1.0f );
			void AddCube( Transformf _transform );

		}
	}
}

#endif