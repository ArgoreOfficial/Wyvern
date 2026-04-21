#pragma once

#include <wv/debug/log.h>

#include <wv/helpers/unordered_array.hpp>

#include <wv/math/vector4.h>

#include <wv/read_through_cache.h>
#include <wv/resource_id.h>

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace wv {

class MaterialAsset;
class TextureAsset;

class IShader
{
public:
	IShader() = default;
	virtual ~IShader();
	
	size_t createMaterial();

	void destroyMaterial( size_t _index );

	void setMaterialData( size_t _index, void* _data );

	void updateMaterialBuffer();

	virtual void initialize() = 0;
	virtual void parseMaterialData( size_t _index, nlohmann::json _json ) { };

	wv::ResourceID getPipelineID() const { return m_pipelineID; }
	wv::ResourceID getBufferID()   const { return m_allocatedBufferID; }

protected:
	void setDebugName( const std::string& _debugName ) { m_debugName = _debugName; }

	void setTopology( wv::TopologyClass _topology ) { m_topology = _topology; }

	void setVertPath( const std::filesystem::path& _path ) {
		m_useVertShader = true;
		m_vertShaderPath = _path;
	}

	void setFragPath( const std::filesystem::path& _path ) {
		m_useFragShader = true;
		m_fragShaderPath = _path;
	}

	void create( size_t _elementSize );

private:
	std::set<size_t> m_materialSlots;
	size_t m_lowestSlot = 0;

	size_t m_elementSize = 0;
	std::vector<uint8_t> m_materialData;

	std::string m_debugName = "";
	wv::TopologyClass m_topology = wv::TopologyClass::WV_TRIANGLE;

	bool m_useVertShader = false;
	bool m_useFragShader = false;

	std::filesystem::path m_vertShaderPath;
	std::filesystem::path m_fragShaderPath;

	wv::ResourceID m_pipelineID{};
	wv::ResourceID m_allocatedBufferID{};
	size_t m_allocatedBufferSize = 0;
};

class MaterialAsset
{
public:
	MaterialAsset() = default;
	MaterialAsset( const std::filesystem::path& _path );

	~MaterialAsset()
	{
		if ( shaderType )
			shaderType->destroyMaterial( m_materialIndex );

		shaderType = {};
		m_materialIndex = SIZE_MAX;

		textureAssets.clear();
	}

	static Ref<MaterialAsset> get( const std::filesystem::path& _path );

	Ref<IShader> shaderType = nullptr;
	size_t m_materialIndex = SIZE_MAX;

	std::vector<Ref<TextureAsset>> textureAssets; // to keep references alive
};

class MaterialManager : public ReadThroughCache<MaterialAsset> { };

class ShaderManager
{
public:
	void add( const std::string& _name, Ref<IShader>& _shader ) {
		if ( m_managed.contains( _name ) )
		{
			WV_LOG_ERROR( "Shader %s already exists\n", _name.c_str() );
			return;
		}

		m_managed.insert( { _name, _shader } );
	}

	Ref<IShader> get( const std::string& _name ) const {
		if ( m_managed.contains( _name ) )
			return m_managed.at( _name );
		return nullptr;
	}

	void updateBuffers() {
		for ( auto& [k,v] : m_managed )
			v->updateMaterialBuffer();
	}

	std::unordered_map<std::string, Ref<IShader>> m_managed;
};










class LitShader : public wv::IShader
{
public:
	struct LitMaterialData
	{
		Vector4f albedoColor;
		uint32_t albedoIndex;
		uint32_t pad0[ 3 ];
	};	

	virtual void initialize() override {
		setDebugName( "Lit" );
		setTopology( wv::TopologyClass::WV_TRIANGLE );
		setVertPath( "shaders/default_lit.vert.spv" );
		setFragPath( "shaders/default_lit.frag.spv" );
		create( sizeof( LitMaterialData ) );
	}

	virtual void parseMaterialData( size_t _index, nlohmann::json _json ) override 
	{
		LitMaterialData data{};

		auto albedo = _json[ "albedo" ];
		if ( albedo.is_string() )
		{
			data.albedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			data.albedoIndex = 0;

			// get texture from albedo path
		}
		else if ( albedo.is_array() )
		{
			data.albedoIndex = 2;
			data.albedoColor = {
				albedo[ 0 ],
				albedo[ 1 ],
				albedo[ 2 ],
				albedo[ 3 ]
			};
		}

		setMaterialData( _index, &data );
	}
};

class UnlitShader : public wv::IShader
{
public:
	struct UnlitMaterialData
	{
		Vector4f albedoColor;
		uint32_t albedoIndex;
		uint32_t pad0[ 3 ];
	};

	virtual void initialize() override {
		setDebugName( "Unlit" );
		setTopology( wv::TopologyClass::WV_TRIANGLE );
		setVertPath( "shaders/default_unlit.vert.spv" );
		setFragPath( "shaders/default_unlit.frag.spv" );
		create( sizeof( UnlitMaterialData ) );
	}

	virtual void parseMaterialData( size_t _index, nlohmann::json _json ) override
	{
		UnlitMaterialData data{};

		auto albedo = _json[ "albedo" ];
		if ( albedo.is_string() )
		{
			data.albedoIndex = 0;
			data.albedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };

			// get texture from albedo path
		}
		else if ( albedo.is_array() )
		{
			data.albedoIndex = 2;
			data.albedoColor = {
				albedo[ 0 ],
				albedo[ 1 ],
				albedo[ 2 ],
				albedo[ 3 ]
			};
		}

		setMaterialData( _index, &data );
	}
};

class DebugShader : public wv::IShader
{
public:
	struct DebugMaterialData
	{
		wv::Vector4f color;
	};

	virtual void initialize() override {
		setDebugName( "Debug" );
		setTopology( wv::TopologyClass::WV_LINE );
		setVertPath( "shaders/default_debug.vert.spv" );
		setFragPath( "shaders/default_debug.frag.spv" );
		create( sizeof( DebugMaterialData ) );
	}

	virtual void parseMaterialData( size_t _index, nlohmann::json _json ) override
	{
		DebugMaterialData data{};

		auto albedo = _json[ "color" ];
		if ( albedo.is_array() )
		{
			data.color = {
				albedo[ 0 ],
				albedo[ 1 ],
				albedo[ 2 ],
				albedo[ 3 ]
			};
		}

		setMaterialData( _index, &data );
	}
};


}