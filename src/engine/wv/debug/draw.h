#pragma once

#include <wv/math/transform.h>
#include <wv/math/vector3.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode;
	class cMaterial;
	class IGraphicsDevice;
	class cResourceRegistry;

///////////////////////////////////////////////////////////////////////////////////////

	namespace Debug 
	{
		namespace Draw
		{

	///////////////////////////////////////////////////////////////////////////////////////

			namespace Internal
			{
				struct Sphere { Vector3f position; float radius; }; /// TODO: move to wv/Math or similar

				extern sMeshNode* SPHERE_MESH;
				extern sMeshNode* CUBE_MESH;
				
				extern std::vector<Sphere> spheres;
				extern std::vector<Transformf> cubes;

				void initDebugDraw( IGraphicsDevice* _pLowLevelGraphics, cResourceRegistry* _pResourceRegistry );
				void deinitDebugDraw( IGraphicsDevice* _pLowLevelGraphics );
				void drawDebug( IGraphicsDevice* _pLowLevelGraphics );
			}

	///////////////////////////////////////////////////////////////////////////////////////

			void AddSphere( Vector3f _position, float _radius = 1.0f );
			void AddCube( Transformf _transform );

		}
	}
}