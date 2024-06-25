#pragma once

#include <wv/Types.h>

#include <wv/Resource/Resource.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cShader;
	
///////////////////////////////////////////////////////////////////////////////////////

	class cShaderProgram : public iResource
	{
	public:
		cShaderProgram( const std::string& _name ) :
			iResource{ _name, L"" }
		{ }

		void addShader( cShader* _shader ) { m_shaders.push_back( _shader ); }
		
		std::vector<cShader*> getShaders() { return m_shaders; }

		void bindUniformToLoc( Uniform _uniform, int _loc );

	private:

		std::vector<cShader*> m_shaders;

	};

}