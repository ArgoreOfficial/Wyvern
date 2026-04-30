#pragma once

#include <wv/debug/log.h>

#include <wv/read_through_cache.h>
#include <wv/resource_id.h>
#include <wv/slot_map.h>

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
	void destroyMaterial( MaterialAsset& _material );
	void setMaterialData( MaterialAsset& _material, void* _data );

	void updateMaterialBuffer();

	virtual void initialize() = 0;
	virtual void parseMaterialData( MaterialAsset& _material, nlohmann::json _json ) { };

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
	SlotMap<uint8_t> m_materials;

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
	MaterialAsset( Ref<IShader> _shader ) {
		shaderType = _shader;
		m_materialIndex = shaderType->createMaterial();
	}

	~MaterialAsset() { 
		destroy(); 
	}

	static Ref<MaterialAsset> get( const std::filesystem::path& _path );

	void destroy();
	void reload();

	std::filesystem::path getPath() const { return path; }

	std::filesystem::path path;

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

}