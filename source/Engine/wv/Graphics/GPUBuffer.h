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
		WV_BUFFER_TYPE_UNIFORM
	};

	enum eGPUBufferUsage
	{
		WV_BUFFER_USAGE_NONE = 0,
		WV_BUFFER_USAGE_STATIC_DRAW,
		WV_BUFFER_USAGE_DYNAMIC_DRAW
	};

	struct sGPUBuffer
	{
		std::string name = "";
		wv::Handle handle = 0;
		eGPUBufferType  type  = WV_BUFFER_TYPE_NONE;
		eGPUBufferUsage usage = WV_BUFFER_USAGE_NONE;

		void* pData = nullptr;

		uint32_t count  = 0;
		uint32_t stride = 0;
		int32_t  size   = 0;
		
		sPlatformData pPlatformData;

		/// <summary>
		/// memcpy helper 
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="_data"></param>
		template<typename T> void buffer( T* _data )
		{
			if( sizeof( T ) > size )
			{
				Debug::Print( Debug::WV_PRINT_ERROR, "Data out of range of shader buffer size\n" );
			}

			memcpy( pData, _data, size );
		}
	};
}