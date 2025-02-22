#pragma once

#include <wv/debug/log.h>
#include <wv/types.h>
#include <wv/graphics/types.h>

namespace wv
{
	enum GPUBufferType
	{
		WV_BUFFER_TYPE_NONE = 0,
		WV_BUFFER_TYPE_INDEX,
		WV_BUFFER_TYPE_VERTEX,
		WV_BUFFER_TYPE_UNIFORM,
		WV_BUFFER_TYPE_DYNAMIC
	};

	enum GPUBufferUsage
	{
		WV_BUFFER_USAGE_NONE = 0,
		WV_BUFFER_USAGE_STATIC_DRAW,
		WV_BUFFER_USAGE_DYNAMIC_DRAW
	};

	struct GPUBufferDesc
	{
		std::string name;
		GPUBufferType type;
		GPUBufferUsage usage;
		int32_t size;
	};

	struct GPUBuffer
	{
		std::string name = "";
		wv::Handle handle = 0;
		GPUBufferType  type  = WV_BUFFER_TYPE_NONE;
		GPUBufferUsage usage = WV_BUFFER_USAGE_NONE;

		uint32_t count  = 0;
		uint32_t stride = 0;
		int32_t  size   = 0;

		wv::Handle blockIndex = 0;
		BufferBindingIndex bindingIndex{};
		
		bool complete = false;

		void* pPlatformData = nullptr;
	};
}