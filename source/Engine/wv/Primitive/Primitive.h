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
		Transformf* pParentTransform;

		sVertexLayout       layout;

		void*    vertices     = nullptr;
		uint32_t sizeVertices = 0;

		uint16_t* indices16  = nullptr;
		uint32_t* indices32  = nullptr;
		uint32_t  numIndices = 0;

		cMaterial* pMaterial = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sMesh
	{
		std::string name;
		Transformf transform;

		wv::Handle handle = 0;
		cGPUBuffer* vertexBuffer = nullptr;
		cGPUBuffer* indexBuffer  = nullptr;

		eMeshDrawType drawType = WV_MESH_DRAW_TYPE_VERTICES;
		
		cMaterial* material = nullptr;

		// remove?
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		void* pPlatformData = nullptr;
	};

}