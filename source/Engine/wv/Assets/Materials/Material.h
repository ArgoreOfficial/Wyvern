#pragma once

#include <vector>
#include <string>

#include <wv/Assets/Materials/MaterialVariable.h>
#include <wv/Math/Matrix.h>
#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cProgramPipeline;
	
	class iGraphicsDevice;
	class Mesh;

///////////////////////////////////////////////////////////////////////////////////////

	struct sUbInstanceData
	{
		cMatrix4x4f projection;
		cMatrix4x4f view;
		cMatrix4x4f model;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cMaterial : public iResource
	{

	public:

		cMaterial( std::string _name, cProgramPipeline* _program ) : 
			iResource( _name, L"" ),
			m_program( _program )
		{ }

		cMaterial( std::string _name, cProgramPipeline* _program, std::vector<sMaterialVariable> _variables ) : 
			iResource( _name, L"" ),
			m_program ( _program),
			m_variables ( _variables)
		{ }

		void setAsActive( iGraphicsDevice* _device );

		void setMaterialUniforms();
		void setInstanceUniforms( Mesh* _instance );

		cProgramPipeline* getProgram() { return m_program; }

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void setDefaultViewUniforms();
		void setDefaultMeshUniforms( Mesh* _mesh );

		cProgramPipeline* m_program = nullptr;
		//std::vector<Texture*> m_textures;
		std::vector<sMaterialVariable> m_variables;

		sUbInstanceData m_UbInstanceData;

	};

}
