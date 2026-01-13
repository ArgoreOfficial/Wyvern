#pragma once

#include <wv/entity/entity_component.h>
#include <wv/types.h>

#include <filesystem>

namespace wv {

class MeshComponent : public IEntityComponent
{
	WV_REFLECT_TYPE( MeshComponent )
public:
	MeshComponent() = default;
	virtual ~MeshComponent();

	void setFilePath( const std::filesystem::path& _path ) { m_path = _path; }
	void setMaterial( ResourceID _material ) { m_material = _material; }

	ResourceID getMeshAsset() const { return m_meshAsset; }
	ResourceID getMaterial() const { return m_material; }

protected:

	virtual void load( WorldLoadContext& _ctx ) override;
	virtual void unload( WorldLoadContext& _ctx ) override;

private:
	std::filesystem::path m_path;

	ResourceID m_meshAsset;
	ResourceID m_material;
};

}