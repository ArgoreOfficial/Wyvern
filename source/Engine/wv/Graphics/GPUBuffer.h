#pragma once

#include <wv/Debug/Print.h>
#include <wv/Types.h>

namespace wv
{
	enum eGPUBufferType
	{
		WV_BUFFER_TYPE_NONE = 0,
		WV_BUFFER_TYPE_INDEX,
		WV_BUFFER_TYPE_VERTEX,
		WV_BUFFER_TYPE_UNIFORM,
		WV_BUFFER_TYPE_DYNAMIC_STORAGE
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
		size_t size = 0;
	};

	class cGPUBuffer
	{
	public:

		template<typename T> 
		void buffer( T* _data, size_t _size = sizeof( T ) )
		{
			if( _size != size )
			{
				if( type == WV_BUFFER_TYPE_DYNAMIC_STORAGE )
				{
					// this may break on platforms that share
					// ram and vram pointers
					// in such cases, vmalloc should be implemented here too
					delete pData;
					pData = malloc( _size );
					size = _size;
					bufferedSize = 0;
				}
				else
					Debug::Print( Debug::WV_PRINT_ERROR, "Data size does not match buffer size\n" );	
			}

			memcpy( pData, _data, size );
		}

		std::string name = "";
		wv::Handle handle = 0;
		eGPUBufferType  type  = WV_BUFFER_TYPE_NONE;
		eGPUBufferUsage usage = WV_BUFFER_USAGE_NONE;

		void* pData = nullptr;

		uint32_t count  = 0;
		uint32_t stride = 0;
		int32_t  size   = 0;

		int32_t bufferedSize = 0;
		
		void* pPlatformData = nullptr;

	};
}