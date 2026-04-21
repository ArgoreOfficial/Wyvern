#include "material.h"

#include <wv/application.h>
#include <wv/filesystem/file_system.h>
#include <wv/rendering/renderer.h>

#include <fstream>

wv::IShader::~IShader()
{
	wv::Application* app = wv::getApp();
	wv::Renderer* renderer = app->getRenderer();

	renderer->destroyGPUBuffer( m_allocatedBufferID );
	renderer->destroyPipeline( m_pipelineID );

	m_allocatedBufferID = { };
	m_allocatedBufferSize = 0;
}

size_t wv::IShader::createMaterial()
{
	size_t slot = m_materials.push( 0 );
	
	// check if cpu side buffer requires resize
	size_t requiredSize = m_materials.size() * m_elementSize + m_elementSize;
	if ( m_materialData.size() < requiredSize )
		m_materialData.resize( requiredSize );

	return slot;
}

void wv::IShader::destroyMaterial( MaterialAsset& _material )
{
	if ( !m_materials.contains( _material.m_materialIndex ) )
		return;

	m_materials.erase( _material.m_materialIndex );
}

void wv::IShader::setMaterialData( MaterialAsset& _material, void* _data )
{
	size_t offset = _material.m_materialIndex * m_elementSize;
	std::memcpy( m_materialData.data() + offset, _data, m_elementSize );
}

void wv::IShader::updateMaterialBuffer()
{
	wv::Application* app = wv::getApp();
	wv::Renderer* renderer = app->getRenderer();

	if ( m_allocatedBufferSize < m_materialData.size() )
	{
		if ( m_allocatedBufferSize > 0 )
			renderer->destroyGPUBuffer( m_allocatedBufferID );

		m_allocatedBufferSize = m_materialData.size();

		std::string debugStr = "Material Buffer : ";
		debugStr += m_debugName;

		m_allocatedBufferID = renderer->createGPUBuffer( m_allocatedBufferSize, debugStr.c_str() );
	}

	renderer->uploadGPUBuffer( m_allocatedBufferID, m_materialData.data(), m_materialData.size() );
}

void wv::IShader::create( size_t _elementSize )
{
	wv::Application* app = wv::getApp();
	wv::Renderer* renderer = app->getRenderer();
	wv::IFileSystem* fs = app->getFileSystem();

	if ( m_useVertShader && m_useFragShader )
	{
		std::vector<uint8_t> fragCode = fs->loadEntireFile( m_fragShaderPath );
		std::vector<uint8_t> vertCode = fs->loadEntireFile( m_vertShaderPath );

		m_pipelineID = renderer->createPipeline(
			(uint32_t*)vertCode.data(), vertCode.size(),
			(uint32_t*)fragCode.data(), fragCode.size(),
			m_topology
		);
	}
	else
	{
		WV_LOG_ERROR( "Vertex and Fragment shaders required for now\n" );
	}

	m_elementSize = _elementSize;
	m_materialData.resize( m_elementSize );
}

wv::MaterialAsset::MaterialAsset( const std::filesystem::path& _path )
{
	auto app = wv::getApp();
	auto fs = app->getFileSystem();

	std::ifstream jfile{ fs->getFullPath( _path ) };
	if ( !jfile )
	{
		WV_LOG_ERROR( "Failed to open material file %s\n", _path.string().c_str() );
		return;
	}

	nlohmann::json j = nlohmann::json::parse( jfile );

	std::string shaderName;
	j[ "shader" ].get_to( shaderName );

	shaderType = app->getShaderManager()->get( shaderName );

	if ( !shaderType )
	{
		WV_LOG_ERROR( "'%s' is not a valid shader type\n", shaderName.c_str() );
		return;
	}

	m_materialIndex = shaderType->createMaterial();

	shaderType->parseMaterialData( *this, j[ "data" ] );

	path = _path;
}

wv::Ref<wv::MaterialAsset> wv::MaterialAsset::get( const std::filesystem::path& _path )
{
	return getApp()->getMaterialManager()->get( _path );
}

void wv::MaterialAsset::reload()
{
	if ( path.empty() )
		return;

	auto app = wv::getApp();
	auto fs = app->getFileSystem();

	std::ifstream jfile{ fs->getFullPath( path ) };
	if ( !jfile )
		return;
	
	nlohmann::json j = nlohmann::json::parse( jfile );

	std::string shaderName;
	j[ "shader" ].get_to( shaderName );

	Ref<IShader> newShaderType = app->getShaderManager()->get( shaderName );

	if ( !newShaderType )
	{
		WV_LOG_ERROR( "'%s' is not a valid shader type\n", shaderName.c_str() );
		return;
	}
	else
	{
		shaderType->destroyMaterial( *this );
		shaderType = newShaderType;
	}

	textureAssets.clear();
	m_materialIndex = shaderType->createMaterial();
	shaderType->parseMaterialData( *this, j[ "data" ] );
}
