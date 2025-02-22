#pragma once

#include <wv/types.h>
#include <wv/graphics/types.h>
#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/math/vector4.h>

#include <wv/graphics/vertex_layout.h>
#include <wv/math/transform.h>

#include <vector>
#include <string>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class Material;
	struct GPUBuffer;

///////////////////////////////////////////////////////////////////////////////////////

	struct Vertex
	{
		wv::Vector3f position;
		wv::Vector3f normal;
		wv::Vector3f tangent;
		wv::Vector4f color;
		wv::Vector2f texCoord0;
	};

///////////////////////////////////////////////////////////////////////////////////////

	enum MeshDrawType
	{
		WV_MESH_DRAW_TYPE_VERTICES,
		WV_MESH_DRAW_TYPE_INDICES
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct MeshDesc
	{
		std::string name;
		Transformf transform;
		Transformf* pParentTransform = nullptr;

		uint8_t* vertices     = nullptr;
		uint32_t sizeVertices = 0;

		uint16_t* pIndices16 = nullptr;
		uint32_t* pIndices32 = nullptr;
		uint32_t  numIndices = 0;

		bool deleteData = true;

		Material* pMaterial = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct Mesh
	{
		std::string name;
		Transformf transform{};

		GPUBufferID vertexBufferID{};
		GPUBufferID indexBufferID{};

		int32_t baseVertex  = 0;
		int32_t numVertices = 0;

		int32_t baseIndex  = 0;
		int32_t numIndices = 0;

		MeshDrawType drawType = WV_MESH_DRAW_TYPE_VERTICES;
		
		Material* pMaterial = nullptr;

		bool complete = false;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct MeshNode
	{
		std::string name;
		Transformf transform;
		std::vector<MeshID> meshes;
		std::vector<MeshNode*> children;
	};

	struct MeshInstanceData
	{
		Matrix4x4f model;                              // 64 bytes
		uint64_t texturesHandles[ 4 ] = { 0, 0, 0, 0 }; // 32 bytes
		int32_t hasAlpha = 0;

		uint8_t padding0[ 12 ] = { 0 };
	};

}