#include "ShaderRegistry.h"

#include <wv/Memory/MemoryDevice.h>

#include <wv/Debug/Print.h>
#include <wv/Device/GraphicsDevice.h>

wv::cShader* wv::cShaderRegistry::loadShader( eShaderType _type, const std::string& _name )
{
	cShader* shader = static_cast<cShader*>( getLoadedResource( _name ) );
	if ( shader )
	{
		shader->incrNumUsers();
		return shader;
	}

	std::wstring fullPath = m_pFileSystem->getFullPath( _name );
	if ( fullPath == L"" ) // file doesn't exist
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Couldn't find file '%s'", _name.c_str() );
		return nullptr;
	}

	std::string shaderSource = m_pFileSystem->loadString( _name );

	shader = m_pGraphicsDevice->createShader( _type ); 
	shader->setSource( shaderSource );
	m_pGraphicsDevice->compileShader( shader );

	addResource( _name, shader );

    return shader;
}
