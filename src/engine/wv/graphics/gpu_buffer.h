#pragma once

#include <wv/Debug/Print.h>
#include <wv/Types.h>
#include <wv/Graphics/Types.h>

namespace wv
{
	enum eGPUBufferType
	{
		WV_BUFFER_TYPE_NONE = 0,
		WV_BUFFER_TYPE_INDEX,
		WV_BUFFER_TYPE_VERTEX,
		WV_BUFFER_TYPE_UNIFORM,
		WV_BUFFER_TYPE_DYNAMIC
	};

	enum eGPUBufferUsage
	{
		WV_BUFFER_USAGE_NONE = 0,
		WV_BUFFER_USAGE_STATIC_DRAW,
		WV_BUFFER_USAGE_DYNAMIC_DRAW
	};

	struct sGPUBufferDesc
	{
		std::string name;
		eGPUBufferType type;
		eGPUBufferUsage usage;
		int32_t size;
	};

	struct sGPUBuffer
	{
		std::string name = "";
		wv::Handle handle = 0;
		eGPUBufferType  type  = WV_BUFFER_TYPE_NONE;
		eGPUBufferUsage usage = WV_BUFFER_USAGE_NONE;

		uint32_t count  = 0;
		uint32_t stride = 0;
		int32_t  size   = 0;

		wv::Handle blockIndex = 0;
		BufferBindingIndex bindingIndex{};
		
		bool complete = false;

		void* pPlatformData = nullptr;
	};
}