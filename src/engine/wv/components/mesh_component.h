#pragma once

#include <wv/serialize.h>
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
	ResourceID materialBuffer = {};
	size_t materialIndex = 0;

	uint32_t firstIndex = 0;
	uint32_t indexCount = 0;
	uint32_t vertexOffset = 0;
};

struct MeshComponent
{
	Ref<MeshAsset> meshAsset;
	std::vector<Ref<MaterialAsset>> materials;
};

template<>
static void serialize<MeshComponent>( SerializeInfo& _info )
{
	_info.name = "MeshComponent";
	_info.registerMember( &MeshComponent::meshAsset, "meshAsset" );
	_info.registerMember( &MeshComponent::materials, "materials" );
}

}