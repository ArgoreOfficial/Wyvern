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

	Ref<IShader> shaderType = nullptr;
	size_t m_materialIndex = SIZE_MAX;

	std::vector<Ref<TextureAsset>> textureAssets; // to keep references alive
};

class MaterialManager : public ReadThroughCache<MaterialAsset> { };
class ShaderManager : public ManagedReadThroughCache<IShader> { };












class LitShader : public wv::IShader
{
public:
	struct LitMaterialData
	{
		uint32_t albedoIndex;
		wv::Vector4f albedoColor;
	};

	virtual void initialize() override {
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
			data.albedoIndex = 0;
			data.albedoColor = { 1.0f,1.0f,1.0f,1.0f };

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






}