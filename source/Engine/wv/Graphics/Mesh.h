#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Types.h>
#include <wv/Math/Vector2.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

#include <wv/Graphics/VertexLayout.h>
#include <wv/Math/Transform.h>

#include <vector>
#include <string>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cMaterial;
	struct sGPUBuffer;

///////////////////////////////////////////////////////////////////////////////////////

	struct Vertex
	{
		wv::cVector3f position;
		wv::cVector3f normal;
		wv::cVector3f tangent;
		wv::cVector4f color;
		wv::Vector2f  texCoord0;
	};

///////////////////////////////////////////////////////////////////////////////////////

	enum eMeshDrawType
	{
		WV_MESH_DRAW_TYPE_VERTICES,
		WV_MESH_DRAW_TYPE_INDICES
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshDesc
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

		cMaterial* pMaterial = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sMesh
	{
		std::string name;
		Transformf transform{};

		int32_t baseVertex  = 0;
		int32_t numVertices = 0;

		int32_t baseIndex  = 0;
		int32_t numIndices = 0;

		eMeshDrawType drawType = WV_MESH_DRAW_TYPE_VERTICES;
		
		cMaterial* pMaterial = nullptr;

		bool complete = false;

		void* pPlatformData = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode
	{
		std::string name;
		Transformf transform;
		std::vector<MeshID> meshes;
		std::vector<sMeshNode*> children;
	};

}