#pragma once

#include <wv/types.h>

#include <wv/rendering/material.h>
#include <wv/math/matrix.h>

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

struct MeshImportOptions
{
	wv::Matrix4x4f transform = wv::Matrix4x4f::identity( 1.0f );
};

class MeshImporterGLTF
{
public:
	MeshImporterGLTF( MeshManager* _meshManager, MaterialManager* _materialManager, TextureManager* _textureManager ) :
		m_meshManager{ _meshManager },
		m_materialManager{ _materialManager },
		m_textureManager{ _textureManager }
	{

	}
	
	void load( const std::filesystem::path& _path, MeshImportOptions _options = {} );

	Ref<MeshAsset> getMesh() const { return m_meshAsset; }
	std::vector<MaterialInstance> getMaterials() const { return m_materials; }
	
private:

	void parseMesh( fastgltf::Asset& _asset, MeshImportOptions _options );

	MeshManager* m_meshManager;
	MaterialManager* m_materialManager;
	TextureManager* m_textureManager;

	Ref<MeshAsset> m_meshAsset = {};
	std::vector<MaterialInstance> m_materials = {};

};

}