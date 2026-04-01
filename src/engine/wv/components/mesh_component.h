#pragma once

// TODO
#include <wv/editor/mesh_importer_gltf.h>

#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>

#include <wv/types.h>

#include <filesystem>
#include <span>

namespace wv {

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
	enum LoadState
	{
		LoadState_unloaded = 0,
		LoadState_loading,
		LoadState_loaded,
		LoadState_loadFailed,
	};

	LoadState loadState = LoadState_unloaded; // internal use, do not set this manually
	MeshImportOptions importOptions{}; 
	std::filesystem::path assetPath{}; 
	
	Ref<MeshAsset> meshAsset;
	std::vector<MaterialInstance> materials;
};

}