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
		~cGPUBuffer()
		{
			if( pData )
			{
				delete pData;
				pData = nullptr;
			}
		}

		template<typename T> 
		void buffer( T* _data, size_t _size = sizeof( T ) )
		{
			if ( _data == nullptr )
				return;

			if( _size >= allocatedSize )
			{
				if( type == WV_BUFFER_TYPE_DYNAMIC_STORAGE )
				{
					// this may break on platforms that share
					// ram and vram pointers
					// in such cases, vmalloc should be implemented here too
					delete[] pData;
					pData = new uint8_t[ _size ];
					allocatedSize = _size;
					bufferedSize = 0;
				}
			}

			size = _size;

			if( pData != nullptr )
				memcpy( pData, _data, size );
		}

		std::string name = "";
		wv::Handle handle = 0;
		eGPUBufferType  type  = WV_BUFFER_TYPE_NONE;
		eGPUBufferUsage usage = WV_BUFFER_USAGE_NONE;

		uint8_t* pData = nullptr;

		uint32_t count  = 0;
		uint32_t stride = 0;
		int32_t  size   = 0;
		int32_t  allocatedSize = 0;

		int32_t bufferedSize = 0;
		
		void* pPlatformData = nullptr;

	};
}