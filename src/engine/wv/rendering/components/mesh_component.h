#pragma once

#include <wv/editor/mesh_importer_gltf.h>

#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>

#include <wv/types.h>

#include <filesystem>

namespace wv {

class TextureAsset;

struct MeshComponent
{
	std::filesystem::path path;

	Ref<MeshAsset> meshAsset;
	std::vector<MaterialInstance> materials;
	
	Ref<TextureAsset> texture;

//	WV_REFLECT_MEMBER( m_path )
//	WV_REFLECT_MEMBER( m_meshAsset )
};

}