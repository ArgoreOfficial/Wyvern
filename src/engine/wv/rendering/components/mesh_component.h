#pragma once

#include <wv/entity/entity_component.h>
#include <wv/types.h>

#include <wv/rendering/material.h>
#include <wv/rendering/mesh.h>

#include <filesystem>

namespace wv {

class TextureAsset;

class MeshComponent : public IEntityComponent
{
	WV_REFLECT_TYPE( MeshComponent, IEntityComponent )
public:
	MeshComponent() = default;
	virtual ~MeshComponent();

	void setFilePath( const std::filesystem::path& _path ) { m_path = _path; }
	
	MeshAsset* getMeshAsset() const { return m_meshAsset.get(); }
	
	const std::vector<MaterialInstance>& getMaterials() const { return m_materials; }

	template<typename Ty>
	void setMaterialValue( size_t _materialIndex, const std::string& _name, const Ty& _value ) {
		m_materials.at( _materialIndex )->setValue<Ty>(_name, _value);
	}

protected:

	virtual void load( WorldLoadContext& _ctx ) override;
	virtual void unload( WorldLoadContext& _ctx ) override;

private:
	std::filesystem::path m_path;

	Ref<MeshAsset> m_meshAsset;
	std::vector<MaterialInstance> m_materials;
	
	Ref<TextureAsset> m_texture;
};

}