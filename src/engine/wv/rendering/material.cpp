#include "material.h"

#include <wv/debug/log.h>

#include <wv/application.h>
#include <wv/rendering/renderer.h>
#include <wv/filesystem/file_system.h>
#include <wv/memory/memory.h>

wv::MaterialAsset::MaterialAsset( const std::filesystem::path& _path )
{ 
	MaterialDefinition def = deserialize( _path );
	initialize( def );
}

wv::MaterialAsset::MaterialAsset( const std::filesystem::path& _vsPath, const std::filesystem::path& _fsPath )
{
	MaterialDefinition def = deserialize( _vsPath, _fsPath );
	initialize( def );
}

wv::MaterialAsset::~MaterialAsset()
{
	Renderer* renderer = Application::getSingleton()->getRenderer();
	renderer->destroyPipeline( m_pipeline );
}

wv::MaterialDefinition wv::MaterialAsset::deserialize( const std::filesystem::path& _path )
{
	WV_LOG_ERROR( __FUNCTION__ " not implemented\n" );
	return {};
}

wv::MaterialDefinition wv::MaterialAsset::deserialize( const std::filesystem::path& _vsPath, const std::filesystem::path& _fsPath )
{
	IFileSystem* fs = Application::getSingleton()->getFileSystem();
	MaterialDefinition def{};

	def.fragShaderPath = _fsPath;
	def.vertShaderPath = _fsPath;

	def.fragCode = fs->loadEntireFile( _fsPath );
	def.vertCode = fs->loadEntireFile( _vsPath );

	return def;
}

void wv::MaterialAsset::initialize( const MaterialDefinition& _def )
{
	Renderer* renderer = Application::getSingleton()->getRenderer();

	if ( !_def.vertCode.empty() && !_def.fragCode.empty() )
	{
		m_pipeline = renderer->createPipeline(
			(uint32_t*)_def.vertCode.data(), _def.vertCode.size(),
			(uint32_t*)_def.fragCode.data(), _def.fragCode.size()
		);
		
		m_rs = {};
		m_rs.pushSpan( "albedoIndex", sizeof( uint32_t ) );
	}

#ifdef WV_DEBUG
	definition = _def;
#endif
}

void wv::MaterialInstance::setValue( const std::string& _name, const void* _data, size_t _dataSize )
{ 
	size_t offset = material->getRS().getMemberOffset(_name);
	if ( offset > buffer.size() || offset + _dataSize > buffer.size() )
	{
		WV_LOG_ERROR( "MaterialInstance setValue out of bounds\n" );
		return;
	}

	std::memcpy( &buffer[ offset ], _data, _dataSize );
}
