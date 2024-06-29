#pragma once

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv 
{

///////////////////////////////////////////////////////////////////////////////////////

	class Mesh;
	class Material;
	class iGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	namespace Debug::Draw
	{

///////////////////////////////////////////////////////////////////////////////////////

		namespace Internal
		{
			struct Sphere { Vector3f position; float radius; }; /// TODO: move to wv/Math or similar
			
			static Mesh* SPHERE_MESH;
			static Mesh* CUBE_MESH;
			static Material* DEBUG_MATERIAL;

			static std::vector<Sphere> spheres;
			static std::vector<Transformf> cubes;

			void initDebugDraw  ( iGraphicsDevice* _pGraphicsDevice );
			void deinitDebugDraw( iGraphicsDevice* _pGraphicsDevice );
			void drawDebug      ( iGraphicsDevice* _pGraphicsDevice );
		}

///////////////////////////////////////////////////////////////////////////////////////

		void AddSphere( Vector3f _position, float _radius = 1.0f );
		void AddCube  ( Transformf _transform );
		
	}
}