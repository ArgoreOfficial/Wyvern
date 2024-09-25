#pragma once

#include <wv/Types.h>
#include <wv/Math/Vector2.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

#include <wv/Graphics/VertexLayout.h>

#include <vector>
#include <string>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cMaterial;
	struct cGPUBuffer;

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

		void*    vertices     = nullptr;
		uint32_t sizeVertices = 0;

		uint16_t* pIndices16 = nullptr;
		uint32_t* pIndices32 = nullptr;
		uint32_t  numIndices = 0;

		cMaterial* pMaterial = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sMesh
	{
		std::string name;
		Transformf transform;

		wv::Handle handle = 0;
		cGPUBuffer* pVertexBuffer = nullptr;
		cGPUBuffer* pVertexPullingBuffer = nullptr;

		cGPUBuffer* pIndexBuffer  = nullptr;

		eMeshDrawType drawType = WV_MESH_DRAW_TYPE_VERTICES;
		
		cMaterial* pMaterial = nullptr;

		// remove?
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		void* pPlatformData = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sMeshNode
	{
		std::string name;
		Transformf transform;
		std::vector<sMesh*> meshes;
		std::vector<sMeshNode*> children;
	};

}