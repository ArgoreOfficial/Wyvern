#pragma once

#include <wv/resource_id.h>
#include <wv/math/vector2.h>
#include <wv/math/vector3.h>

#include <filesystem>
#include <vector>

namespace wv {

struct GeometrySurface
{
	size_t indexCount()  const { return indices.size(); }
	size_t vertexCount() const { return vertexPositions.size(); }

	std::vector<uint16_t> indices;

	std::vector<Vector3f> vertexPositions;
	std::vector<Vector3f> vertexNormals;
	std::vector<Vector2f> vertexUVs;
	std::vector<Vector3f> vertexColours;
};

class MeshAsset
{
	friend class MeshManager;

public:
	MeshAsset() = default;
	MeshAsset( const std::filesystem::path& _path );
	~MeshAsset();

	GeometrySurface deserialize( const std::filesystem::path& _path );
	GeometrySurface deserializeGltf( const std::filesystem::path& _path );

	void initialize( const GeometrySurface& _geometry );

	ResourceID getGPUAllocation() const { return m_gpuAllocation; }

private:
	std::filesystem::path m_path;

	GeometrySurface m_surface{};
	ResourceID m_gpuAllocation{};
};

}