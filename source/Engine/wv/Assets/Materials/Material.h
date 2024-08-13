#pragma once

#include <vector>
#include <string>

#include <wv/Assets/Materials/MaterialVariable.h>

#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderProgram;
	
	class iGraphicsDevice;
	class Mesh;

///////////////////////////////////////////////////////////////////////////////////////

	class cMaterial : public iResource
	{

	public:

		cMaterial( std::string _name, cShaderProgram* _program ) :
			iResource{ _name, L"" },
			m_program{ _program }
		{ }

		void setAsActive( iGraphicsDevice* _device );

		void setMaterialUniforms();
		void setInstanceUniforms( Mesh* _instance );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void setDefaultViewUniforms();
		void setDefaultMeshUniforms( Mesh* _mesh );

		cShaderProgram* m_program = nullptr;
		//std::vector<Texture*> m_textures;
		std::vector<sMaterialVariable> m_variables;

	};

}
