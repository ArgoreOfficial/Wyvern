#pragma once

namespace wv
{
	enum eGPUBufferType
	{
		WV_BUFFER_TYPE_INDEX,
		WV_BUFFER_TYPE_VERTEX
	};

	struct sGPUBuffer
	{
		wv::Handle handle = 0;
		eGPUBufferType type;

		void* data = nullptr;

		uint32_t count = 0;
		uint32_t stride = 0;
		uint32_t size = 0;

	};
}