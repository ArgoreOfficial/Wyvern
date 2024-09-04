#pragma once

#include <wv/Types.h>

#include <wv/Resource/Resource.h>

#include <wv/Shader/UniformBlock.h>
//#include <wv/Debug/Print.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cShader;
	class cShaderRegistry;
	class iGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderProgram : public iResource
	{
	public:
		cShaderProgram( cShaderRegistry* _pShaderRegistry, const std::string& _name ) :
			iResource( _name, L"" ),
			m_pShaderRegistry( _pShaderRegistry )
		{ }

		void addShader( cShader* _shader ) { m_shaders.push_back( _shader ); }
		
		std::vector<cShader*> getShaders() { return m_shaders; }

		void load  ( cFileSystem* _pFileSystem ) override;
		void unload( cFileSystem* _pFileSystem ) override;
		
		void create ( iGraphicsDevice* _pGraphicsDevice ) override;
		void destroy( iGraphicsDevice* _pGraphicsDevice ) override;

		void bindUniformToLoc( Uniform _uniform, int _loc );

		UniformBlock* getUniformBlock( const std::string& _name );
		UniformBlockMap* getUniformBlockMap() { return &m_uniformBlocks; }

		void addUniformBlock( const std::string& _name, const UniformBlock& _block );

	private:
		cShaderRegistry* m_pShaderRegistry;

		std::vector<cShader*> m_shaders;

		std::vector<UniformBlockDesc> m_uniformBlockDescs;
		std::vector<Uniform> m_textureUniforms;

		UniformBlockMap m_uniformBlocks;

	};

}