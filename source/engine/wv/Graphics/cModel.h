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
		 cModel( void ) { }
		~cModel( void );

		void create( std::string _path );
		void render();

		std::vector<cMesh*> meshes;
		
		wv::cTransformf transform;
		std::string path;
	};

}