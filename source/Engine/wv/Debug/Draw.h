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
	class iLowLevelGraphics;
	class cResourceRegistry;

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
				
				static std::vector<Sphere> spheres;
				static std::vector<Transformf> cubes;

				void initDebugDraw( iLowLevelGraphics* _pLowLevelGraphics, cResourceRegistry* _pResourceRegistry );
				void deinitDebugDraw( iLowLevelGraphics* _pLowLevelGraphics );
				void drawDebug( iLowLevelGraphics* _pLowLevelGraphics );
			}

	///////////////////////////////////////////////////////////////////////////////////////

			void AddSphere( cVector3f _position, float _radius = 1.0f );
			void AddCube( Transformf _transform );

		}
	}
}