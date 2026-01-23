#pragma once

#include <wv/entity/entity_component.h>
#include <wv/types.h>

#include <wv/rendering/material.h>

#include <filesystem>

namespace wv {

class MaterialType;

class MeshComponent : public IEntityComponent
{
	WV_REFLECT_TYPE( MeshComponent, IEntityComponent )
public:
	MeshComponent() = default;
	virtual ~MeshComponent();

	void setFilePath( const std::filesystem::path& _path ) { m_path = _path; }
	void setMaterial( MaterialType* _material );

	ResourceID getMeshAsset() const { return m_meshAsset; }
	ResourceID getMaterial() const { return m_materialInstance; }

	MaterialType* getMaterialType() { return m_materialType; }

	template<typename Ty>
	void setMaterialValue( size_t _materialIndex, const std::string& _name, const Ty& _value ) {
		if ( m_materialType == nullptr )
			return;
		m_materialType->setValue<Ty>( m_materialInstance, _name, _value );
	}

protected:

	virtual void load( WorldLoadContext& _ctx ) override;
	virtual void unload( WorldLoadContext& _ctx ) override;

private:
	std::filesystem::path m_path;

	ResourceID m_meshAsset;

	MaterialType* m_materialType = nullptr;
	ResourceID m_materialInstance;
};

}