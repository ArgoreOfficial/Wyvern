#pragma once

#include <wv/Graphics/cMaterial.h>

#include <wv/Graphics/cMesh.h>
#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>

#include <string>

namespace wv 
{

	class cModel
	{
	public:
		 cModel();
		~cModel();

		void create( std::string _path );
		void render();

	private:
		cMesh*          m_mesh;
		cMaterial       m_material;
		wv::cTransformf m_transform;

	};

}