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
			iResource( _name, L"" ),
			m_program( _program )
		{ }

		cMaterial( std::string _name, cShaderProgram* _program, std::vector<sMaterialVariable> _variables ) : 
			iResource( _name, L"" ),
			m_program ( _program),
			m_variables ( _variables)
		{ }

		void setAsActive( iGraphicsDevice* _device );

		void setMaterialUniforms();
		void setInstanceUniforms( Mesh* _instance );

		cShaderProgram* getProgram() { return m_program; }

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void setDefaultViewUniforms();
		void setDefaultMeshUniforms( Mesh* _mesh );

		cShaderProgram* m_program = nullptr;
		//std::vector<Texture*> m_textures;
		std::vector<sMaterialVariable> m_variables;

	};

}
