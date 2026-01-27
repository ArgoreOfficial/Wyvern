#pragma once

#include <wv/resource_id.h>
#include <wv/helpers/unordered_array.hpp>
#include <wv/read_through_cache.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace wv {

enum UniformType
{
	UNIFORM_TYPE_FLOAT,
	UNIFORM_TYPE_VEC2,
	UNIFORM_TYPE_VEC3,
	UNIFORM_TYPE_VEC4,
	UNIFORM_TYPE_TEXTURE
};

class MaterialType;

struct MaterialInstance
{
	MaterialType* type = nullptr;

	std::vector<uint8_t> buffer = {};
};

class MaterialType
{
public:
	struct UniformSpan
	{
		std::string name;
		UniformType type;
		uint32_t offset;
	};

	MaterialType( ResourceID _pipeline = {} ) : m_pipeline{ _pipeline } { }

	void addSpan( const std::string& _name, UniformType _type );

	ResourceID createInstance();
	void destroyInstance( ResourceID _instance );

	template<typename Ty>
	void setValue( ResourceID _materialInstance, const std::string& _name, const Ty& _value ) {
		setValueInternal( _materialInstance, _name, &_value, sizeof( Ty ) );
	}

	void setPipeline( ResourceID _pipeline ) { m_pipeline = _pipeline; }
	ResourceID getPipeline() const { return m_pipeline; }

	uint8_t* getBuffer( ResourceID _materialInstance ) {
		if ( !m_instances.contains( _materialInstance ) )
			return nullptr;
		return m_instances.at( _materialInstance ).buffer.data();
	}

	size_t getBufferSize( ResourceID _materialInstance ) {
		if ( !m_instances.contains( _materialInstance ) )
			return 0;
		return m_instances.at( _materialInstance ).buffer.size();
	}

private:
	void setValueInternal( ResourceID _materialInstance, const std::string& _name, const void* _data, size_t _dataSize );

	uint32_t uniformTypeSize( UniformType _type ) {
		switch ( _type )
		{
		case UNIFORM_TYPE_FLOAT: return sizeof( float ); break;
		case UNIFORM_TYPE_VEC2: return sizeof( Vector2f ); break;
		case UNIFORM_TYPE_VEC3: return sizeof( Vector3f ); break;
		case UNIFORM_TYPE_VEC4: return sizeof( Vector3f ); break;
		case UNIFORM_TYPE_TEXTURE: return sizeof( uint32_t ); break;
		}

		return 0;
	}

	ResourceID m_pipeline = {};

	size_t m_bufferSize = 0;
	std::vector<UniformSpan> m_uniforms = {};
	std::unordered_map<std::string, size_t> m_uniformNameMap = {};

	unordered_array<ResourceID, MaterialInstance> m_instances = {};
};

struct MaterialDefinition
{
	std::filesystem::path path;

	std::filesystem::path vertShaderPath;
	std::filesystem::path fragShaderPath;

	std::vector<uint8_t> vertCode;
	std::vector<uint8_t> fragCode;
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

	MaterialDefinition definition;
	MaterialType* materialType = nullptr;
};

class MaterialManager : public ReadThroughCache<MaterialAsset> { };

}