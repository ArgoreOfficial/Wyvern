#include "material.h"

#include <wv/debug/log.h>

#include <wv/application.h>
#include <wv/rendering/renderer.h>
#include <wv/filesystem/file_system.h>
#include <wv/memory/memory.h>

void wv::MaterialType::addSpan( const std::string& _name, UniformType _type )
{
	if ( m_uniformNameMap.contains( _name ) )
	{
		WV_LOG_ERROR( "Uniform %s already exists\n", _name.c_str() );
		return;
	}

	uint32_t offset = 0;

	if ( m_uniforms.size() > 0 )
	{
		offset += m_uniforms.back().offset;
		offset += uniformTypeSize( m_uniforms.back().type );
	}

	m_uniformNameMap[ _name ] = m_uniforms.size();
	m_uniforms.push_back( UniformSpan{ _name, _type, offset } );

	m_bufferSize += offset + uniformTypeSize( _type );
}

wv::ResourceID wv::MaterialType::createInstance()
{
	MaterialInstance instance{};
	instance.type = this;
	instance.buffer.resize( m_bufferSize );
	
	return m_instances.emplace( instance );
}

void wv::MaterialType::destroyInstance( ResourceID _instance )
{ 
	if ( !m_instances.contains( _instance ) )
	{
		WV_LOG_ERROR( "Material instance does not exist\n" );
		return;
	}

	m_instances.erase( _instance );
}

void wv::MaterialType::setValueInternal( ResourceID _materialInstance, const std::string& _name, const void* _data, size_t _dataSize )
{
	if ( !m_instances.contains( _materialInstance ) )
	{
		WV_LOG_ERROR( "Material instance does not exist\n" );
		return;
	}

	if ( !m_uniformNameMap.contains( _name ) )
	{
		WV_LOG_ERROR( "Uniform '%s' does not exist\n", _name.c_str() );
		return;
	}

	UniformSpan span = m_uniforms[ m_uniformNameMap.at( _name ) ];
	if ( _dataSize != uniformTypeSize( span.type ) )
	{
		WV_LOG_ERROR( "Uniform '%s' size error\n", _name.c_str() );
		return;
	}

	MaterialInstance& instance = m_instances.at( _materialInstance );
	
	if ( span.offset + _dataSize > instance.buffer.size() )
	{
		WV_LOG_ERROR( "Uniform '%s' out of bounds\n", _name.c_str() );
		return;
	}

	std::memcpy( &instance.buffer[ span.offset ], _data, _dataSize );
}


wv::MaterialAsset::~MaterialAsset()
{
	if ( materialTypeDefinition )
	{
		Renderer* renderer = Application::getSingleton()->getRenderer();
		renderer->destroyPipeline( materialTypeDefinition->getPipeline() );
		WV_FREE( materialTypeDefinition );
	}
}

void wv::MaterialAsset::initialize()
{
	IFileSystem* fs = Application::getSingleton()->getFileSystem();
	Renderer* renderer = Application::getSingleton()->getRenderer();

	if ( !fragShaderPath.empty() )
		fragCode = fs->loadEntireFile( fragShaderPath );

	if ( !vertShaderPath.empty() )
		vertCode = fs->loadEntireFile( vertShaderPath );

	if ( !vertCode.empty() && !fragCode.empty() )
	{
		ResourceID pipeline = renderer->createPipeline(
			(uint32_t*)vertCode.data(), vertCode.size(),
			(uint32_t*)fragCode.data(), fragCode.size()
		);

	#ifndef WV_DEBUG
		fragCode.clear();
		vertCode.clear();
	#endif

		materialTypeDefinition = WV_NEW( MaterialType );
		materialTypeDefinition->setPipeline( pipeline );
		materialTypeDefinition->addSpan( "albedoIndex", UNIFORM_TYPE_TEXTURE );
	}

	isLoaded = true;
}
