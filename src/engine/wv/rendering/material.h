#pragma once

#include <wv/debug/log.h>

#include <wv/helpers/unordered_array.hpp>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <wv/reflection/runtime_structure.h>

#include <wv/read_through_cache.h>
#include <wv/resource_id.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace wv {

class TextureAsset;

struct MaterialUniform
{
	std::string name;
	size_t size;
};

struct MaterialDefinition
{
	std::filesystem::path path;

	std::filesystem::path vertShaderPath;
	std::filesystem::path fragShaderPath;

	std::vector<uint8_t> vertCode;
	std::vector<uint8_t> fragCode;

	std::vector<MaterialUniform> uniforms;
	TopologyClass topology = TopologyClass::WV_TRIANGLE;
};

class MaterialAsset
{
public:
	MaterialAsset() = default;
	MaterialAsset( const std::filesystem::path& _path );
	MaterialAsset( const std::filesystem::path& _vsPath, const std::filesystem::path& _fsPath );
	~MaterialAsset();

	MaterialDefinition deserialize( const std::filesystem::path& _path );
	MaterialDefinition deserialize( const std::filesystem::path& _vsPath, const std::filesystem::path& _fsPath );

	void initialize( const MaterialDefinition& _def );

	void load( const std::filesystem::path& _path ) {
		initialize( deserialize( _path ) );
	}

	ResourceID getPipeline() const { return m_pipeline; }

	const RuntimeStructure& getRS() const { return m_rs; }

protected:
	ResourceID m_pipeline = {};

	MaterialDefinition m_definition = {};
	RuntimeStructure m_rs = {};
};

struct MaterialInstance
{
	MaterialInstance() = default;
	MaterialInstance( const Ref<MaterialAsset>& _material ) : material{ _material }
	{ 
		buffer.resize( _material->getRS().getSize() );
	}

	void setValue( const std::string& _name, const void* _data, size_t _dataSize );

	template<typename Ty>
	void setValue( const std::string& _name, const Ty& _value ) {
		setValue( _name, &_value, sizeof( Ty ) );
	}

	Ref<MaterialAsset> material;
	std::vector<uint8_t> buffer = {};
	std::vector<Ref<TextureAsset>> textures = {};
};

class MaterialManager : public ReadThroughCache<MaterialAsset> { };

}