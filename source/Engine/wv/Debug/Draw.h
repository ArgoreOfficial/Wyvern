#pragma once

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode;
	class cMaterial;
	class iGraphicsDevice;
	class cMaterialRegistry;

///////////////////////////////////////////////////////////////////////////////////////

	namespace Debug 
	{
		namespace Draw
		{

	///////////////////////////////////////////////////////////////////////////////////////

			namespace Internal
			{
				struct Sphere { cVector3f position; float radius; }; /// TODO: move to wv/Math or similar

				static sMeshNode* SPHERE_MESH;
				static sMeshNode* CUBE_MESH;
				static cMaterial* DEBUG_MATERIAL;

				static std::vector<Sphere> spheres;
				static std::vector<Transformf> cubes;

				void initDebugDraw( iGraphicsDevice* _pGraphicsDevice, cMaterialRegistry* _pMaterialRegistry );
				void deinitDebugDraw( iGraphicsDevice* _pGraphicsDevice );
				void drawDebug( iGraphicsDevice* _pGraphicsDevice );
			}

	///////////////////////////////////////////////////////////////////////////////////////

			void AddSphere( cVector3f _position, float _radius = 1.0f );
			void AddCube( Transformf _transform );

		}
	}
}