#pragma once

#include <wv/Types.h>

#include <wv/Debug/Print.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	enum eUniformBlockSubmitMode
	{
		WV_UNIFORM_BLOCK_SUBMIT_IMMEDIATE,
		WV_UNIFORM_BLOCK_SUBMIT_DEFERRED
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sUniform
	{
		unsigned int index = 0;
		unsigned int offset = 0;
		std::string name;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sShaderBufferDesc
	{
		std::string name;
		int index;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderBuffer
	{
	public:
		cShaderBuffer() { }
	
		template<typename T> void buffer( T* _data );

///////////////////////////////////////////////////////////////////////////////////////
		std::string name = "";

		wv::Handle m_index = 0;
		wv::Handle m_bufferHandle = 0;
		wv::Handle m_bindingIndex = 0;

		char* m_buffer = nullptr;
		int m_bufferSize = 0;

		PlatformData m_pPlatformData = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline void cShaderBuffer::buffer( T* _data )
	{
		if( sizeof( T ) > m_bufferSize )
		{
			Debug::Print( Debug::WV_PRINT_ERROR, "Data out of range of shader buffer size\n" );
		}
		
		memcpy( m_buffer, _data, m_bufferSize );
	}

}