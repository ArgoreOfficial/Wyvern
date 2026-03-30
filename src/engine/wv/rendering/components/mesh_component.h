#pragma once

#include <wv/editor/mesh_importer_gltf.h>

#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>

#include <wv/types.h>

#include <filesystem>
#include <span>

namespace wv {

class TextureAsset;

struct RenderMesh
{
	ResourceID mesh = {};
	ResourceID pipeline = {};
	uint32_t firstIndex = 0;
	uint32_t indexCount = 0;
	uint32_t vertexOffset = 0;
	std::span<const uint8_t> materialData = {};
};

struct MeshComponent
{
	std::filesystem::path path;
	
	Ref<MeshAsset> meshAsset;
	std::vector<MaterialInstance> materials;

	Ref<TextureAsset> texture;
};

}