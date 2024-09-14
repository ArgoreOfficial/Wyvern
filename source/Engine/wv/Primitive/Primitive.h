#pragma once

#include <wv/Types.h>
#include <wv/Math/Vector2.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

#include <wv/Graphics/VertexLayout.h>

#include <vector>

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
		wv::Vector2f texCoord0;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sVertexBuffer
	{
		wv::Handle handle = 0;
		uint32_t count = 0;
		uint32_t size = 0;
	};

	struct sIndexBuffer
	{
		wv::Handle handle = 0;
		uint32_t numIndices = 0;
		uint32_t size = 0;
	};
	
///////////////////////////////////////////////////////////////////////////////////////

	enum PrimitiveBufferMode
	{
		WV_PRIMITIVE_TYPE_STATIC
	};

///////////////////////////////////////////////////////////////////////////////////////

	enum PrimitiveDrawType
	{
		WV_PRIMITIVE_DRAW_TYPE_VERTICES,
		WV_PRIMITIVE_DRAW_TYPE_INDICES
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct PrimitiveDesc
	{
		PrimitiveBufferMode type   = WV_PRIMITIVE_TYPE_STATIC;
		sVertexLayout       layout;

		void*    vertices     = nullptr;
		uint32_t sizeVertices = 0;

		uint16_t* indices16  = nullptr;
		uint32_t* indices32  = nullptr;
		uint32_t  numIndices = 0;

		cMaterial* pMaterial = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class Primitive
	{

	public:

		wv::Handle vaoHandle = 0;
		cGPUBuffer* vertexBuffer;
		cGPUBuffer* indexBuffer;

		PrimitiveBufferMode mode = WV_PRIMITIVE_TYPE_STATIC;
		PrimitiveDrawType drawType = WV_PRIMITIVE_DRAW_TYPE_VERTICES;
		
		cMaterial* material = nullptr;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		void* pPlatformData;
	};

}