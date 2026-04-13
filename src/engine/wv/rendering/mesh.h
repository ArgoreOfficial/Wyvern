#pragma once

#include <wv/resource_id.h>
#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/math/matrix.h>

#include <wv/read_through_cache.h>
#include <wv/reflection/reflection.h>
#include <wv/debug/log.h>

#include <filesystem>
#include <vector>

namespace wv {

struct GeometrySurface
{
	struct Primitive
	{
		uint32_t firstIndex = 0;
		uint32_t vertexOffset = 0;

		uint32_t indexCount = 0;
		uint32_t vertexCount = 0;
		
		int material = 0;
	};

	size_t indexCount()  const { return indices.size(); }
	size_t vertexCount() const { return vertexPositions.size(); }

	std::vector<Primitive> primitives;
	
	std::vector<uint32_t> indices;

	std::vector<Vector3f> vertexPositions;
	std::vector<Vector3f> vertexNormals;
	std::vector<Vector2f> vertexUVs;
	std::vector<Vector3f> vertexColours;
};

class MeshAsset : public IReflectedType
{
	friend class MeshImporterGLTF;

	WV_REFLECT_TYPE( MeshAsset, IReflectedType )

public:
	MeshAsset() = default;
	MeshAsset( const std::filesystem::path& _path );
	~MeshAsset();

	static Ref<MeshAsset> get( const std::filesystem::path& _path );

	void serialize( const std::filesystem::path& _path );
	GeometrySurface deserialize( const std::filesystem::path& _path );
	void initialize( const GeometrySurface& _geometry );

	void load( const std::filesystem::path& _path ) {
		Debug::Print( Debug::WV_PRINT_DEBUG, "Loading Mesh %s\n", _path.string().c_str() );
		initialize( deserialize( _path ) );
	}

	ResourceID getGPUAllocation() const { return m_gpuAllocation; }
	const std::vector<GeometrySurface::Primitive>& getPrimitives() const { return m_primitives; }
	uint32_t getNumMaterials() const { return m_numMaterials; }

private:
	std::filesystem::path m_path;

	std::vector<GeometrySurface::Primitive> m_primitives;

	int m_numMaterials = 0;

	GeometrySurface m_surface{};
	ResourceID m_gpuAllocation{};
};

class MeshManager : public ReadThroughCache<MeshAsset> { };

}