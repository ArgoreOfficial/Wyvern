#pragma once

#include <vector>
#include <string>

#include <wv/Resource/Resource.h>

#include <wv/Auxiliary/fkYAML/node.hpp>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderProgram;
	
	class iGraphicsDevice;
	class Mesh;
	class Texture;

///////////////////////////////////////////////////////////////////////////////////////

	class iMaterial : public iResource
	{

	public:

		iMaterial( std::string _name, cShaderProgram* _program ) :
			iResource{ _name, L"" },
			m_program{ _program }
		{ }

		void setAsActive( iGraphicsDevice* _device );

		virtual void setMaterialUniforms();
		virtual void setInstanceUniforms( Mesh* _instance );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void setDefaultViewUniforms();
		void setDefaultMeshUniforms( Mesh* _mesh );

		cShaderProgram* m_program = nullptr;
		//std::vector<Texture*> m_textures;

	};

}
