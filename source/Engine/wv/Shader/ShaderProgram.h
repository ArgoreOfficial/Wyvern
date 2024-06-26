#pragma once

#include <wv/Types.h>

#include <wv/Resource/Resource.h>

#include <wv/Shader/UniformBlock.h>
#include <wv/Debug/Print.h>

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

		// void bindUniformToLoc( Uniform _uniform, int _loc );

		UniformBlock* getUniformBlock( const std::string& _name )
		{
			if ( !m_uniformBlocks.contains( _name ) )
				return nullptr;

			return &m_uniformBlocks[ _name ];
		}
		
		UniformBlockMap* getUniformBlockMap() { return &m_uniformBlocks; }

		/// TODO: not inline
		void addUniformBlock( const std::string& _name, const UniformBlock& _block ) 
		{
			if ( m_uniformBlocks.contains( _name ) )
			{
				Debug::Print( Debug::WV_PRINT_ERROR, "Uniform Block '%s' already exists\n", _name.c_str() );
				return;
			}

			m_uniformBlocks[ _name ] = _block;
		}

	private:

		std::vector<cShader*> m_shaders;

		UniformBlockMap m_uniformBlocks;

	};

}