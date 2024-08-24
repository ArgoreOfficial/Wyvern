#pragma once

#include <wv/Types.h>
#include <wv/Math/Vector2.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

#include <vector>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cMaterial;
	struct sGPUBuffer;

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

	struct InputLayoutElement
	{
		unsigned int num;
		DataType type;
		bool normalized;
		unsigned int size;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct InputLayout
	{
		InputLayoutElement* elements;
		unsigned int numElements;
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
		PrimitiveBufferMode type = WV_PRIMITIVE_TYPE_STATIC;
		InputLayout* layout = nullptr;

		std::vector<Vertex>   vertices;
		std::vector<uint32_t> indices;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class Primitive
	{

	public:

		wv::Handle vaoHandle = 0;
		sGPUBuffer* vertexBuffer;
		sGPUBuffer* indexBuffer;

		PrimitiveBufferMode mode = WV_PRIMITIVE_TYPE_STATIC;
		PrimitiveDrawType drawType = WV_PRIMITIVE_DRAW_TYPE_VERTICES;
		
		cMaterial* material = nullptr;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

}