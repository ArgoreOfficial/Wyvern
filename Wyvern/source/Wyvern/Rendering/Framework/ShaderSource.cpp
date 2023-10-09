#include "ShaderSource.h"
using namespace WV;

ShaderSource::ShaderSource( std::string _path )
{ 
	m_path = _path;

	std::ifstream stream( _path );
	std::string line;
	
	enum ShaderType { NONE, VERT, FRAG };
	ShaderType shaderType = ShaderType::NONE;

	while ( getline( stream, line ) )
	{
		if ( line.find( "shader" ) != std::string::npos )
		{
			if ( line.find( "vertex" ) != std::string::npos )
			{
				shaderType = ShaderType::VERT;
			}
			else if ( line.find( "fragment" ) != std::string::npos )
			{
				shaderType = ShaderType::FRAG;
			}
		}
		else
		{
			if( shaderType == ShaderType::VERT )
				m_vertexSource += line + "\n";
			else if ( shaderType == ShaderType::FRAG )
				m_fragmentSource += line + "\n";
		}

	}
}

ShaderSource::~ShaderSource()
{ }
