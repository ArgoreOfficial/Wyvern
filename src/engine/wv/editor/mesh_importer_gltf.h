#pragma once

#include <wv/types.h>

#include <wv/rendering/material.h>

#include <filesystem>
#include <vector>

namespace fastgltf {
class Asset;

}

namespace wv {

class MeshAsset;
class MaterialAsset;
class TextureAsset;

class MeshManager;
class MaterialManager;
class TextureManager;

class MeshImporterGLTF
{
public:
	MeshImporterGLTF() = default;
	
	void load( const std::filesystem::path& _path, MeshManager* _meshManager, MaterialManager* _materialManager, TextureManager* _textureManager );

	Ref<MeshAsset> getMesh() const { return m_meshAsset; }
	std::vector<MaterialInstance> getMaterials() const { return m_materials; }
	
private:

	void parseMesh( fastgltf::Asset& _asset );

	Ref<MeshAsset> m_meshAsset = {};
	std::vector<MaterialInstance> m_materials = {};

};

}