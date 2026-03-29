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
	MeshImportOptions importOptions{};

	std::filesystem::path m_path;

	Ref<MeshAsset> m_meshAsset;
	std::vector<MaterialInstance> m_materials;
	
	Ref<TextureAsset> m_texture;

//	WV_REFLECT_MEMBER( m_path )
//	WV_REFLECT_MEMBER( m_meshAsset )
};

}