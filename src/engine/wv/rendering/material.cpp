#include "material.h"

#include <wv/debug/log.h>

#include <wv/application.h>
#include <wv/rendering/renderer.h>
#include <wv/filesystem/file_system.h>
#include <wv/memory/memory.h>

#include <nlohmann/json.hpp>

#include <fstream>

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

void wv::MaterialAsset::serialize( const std::filesystem::path& _path )
{
	std::string topology = "Triangle";
	switch ( m_definition.topology )
	{
	case TopologyClass::WV_POINT: topology = "Point"; break;
	case TopologyClass::WV_LINE:  topology = "Line";  break;
	}

	std::vector<nlohmann::ordered_json> uniforms;
	for ( auto& u : m_definition.uniforms )
	{
		uniforms.push_back(
			{
				{ "name", u.name },
				{ "size", u.size },
			} );
	}

	nlohmann::ordered_json j{
		{ "vertShaderPath", m_definition.vertShaderPath },
		{ "fragShaderPath", m_definition.fragShaderPath },
		{ "topology", topology },
		{ "uniforms", uniforms }
	};

	std::filesystem::path path = _path;
	path.replace_extension( ".wvmt" );

	std::ofstream stream{ path };
	stream << j.dump( 1 );
}

wv::MaterialDefinition wv::MaterialAsset::deserialize( const std::filesystem::path& _path )
{
	IFileSystem* fs = getApp()->getFileSystem();

	std::filesystem::path path = _path;
	path = fs->getFullPath( path.replace_extension( ".wvmt" ) );

	std::ifstream f{ path };
	if ( !f )
		return {};

	nlohmann::json j = nlohmann::json::parse( f );
	
	std::string topology{};
	MaterialDefinition def{};
	def.path = _path;

	j.at( "vertShaderPath" ).get_to( def.vertShaderPath );
	j.at( "fragShaderPath" ).get_to( def.fragShaderPath );
	j.at( "topology" ).get_to( topology );

	     if ( topology == "Triangle" ) def.topology = TopologyClass::WV_TRIANGLE;
	else if ( topology == "Line" )     def.topology = TopologyClass::WV_LINE;
	else if ( topology == "Point" )    def.topology = TopologyClass::WV_POINT;

	for ( nlohmann::json& u : j[ "uniforms" ] )
	{
		MaterialUniform uniform{};
		u.at( "name" ).get_to( uniform.name );
		u.at( "size" ).get_to( uniform.size );
		def.uniforms.push_back( uniform );
	}

	def.vertCode = fs->loadEntireFile( def.vertShaderPath );
	def.fragCode = fs->loadEntireFile( def.fragShaderPath );

	return def;
}

wv::MaterialDefinition wv::MaterialAsset::deserialize( const std::filesystem::path& _vsPath, const std::filesystem::path& _fsPath )
{
	IFileSystem* fs = Application::getSingleton()->getFileSystem();
	MaterialDefinition def{};

	def.fragShaderPath = _fsPath;
	def.vertShaderPath = _vsPath;

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
			(uint32_t*)_def.fragCode.data(), _def.fragCode.size(),
			_def.topology
		);
		
		m_rs = {};

		for ( const MaterialUniform& uniform : _def.uniforms )
			m_rs.pushSpan( uniform.name, uniform.size );
		
		//m_rs.pushSpan( "albedoIndex", sizeof( uint32_t ) );
	}

#ifdef WV_DEBUG
	m_definition = _def;
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
