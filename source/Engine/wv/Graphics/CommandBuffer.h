#pragma once

#include <stdint.h>

#include <wv/Memory/Memory.h> // memory stream
#include <wv/Memory/Function.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	WV_DEFINE_ID( CmdBufferID );

///////////////////////////////////////////////////////////////////////////////////////

	enum eGPUTaskType : uint8_t
	{
		WV_GPUTASK_NONE = 0,
		
		WV_GPUTASK_CREATE_RENDERTARGET,
		WV_GPUTASK_DESTROY_RENDERTARGET,
		WV_GPUTASK_SET_RENDERTARGET,
		WV_GPUTASK_CLEAR_RENDERTARGET,

		WV_GPUTASK_CREATE_PROGRAM,
		WV_GPUTASK_DESTROY_PROGRAM,

		WV_GPUTASK_CREATE_PIPELINE,
		WV_GPUTASK_DESTROY_PIPELINE,
		WV_GPUTASK_BIND_PIPELINE,

		WV_GPUTASK_CREATE_BUFFER,
		WV_GPUTASK_BUFFER_DATA,
		WV_GPUTASK_DESTROY_BUFFER,

		WV_GPUTASK_CREATE_MESH,
		WV_GPUTASK_DESTROY_MESH,

		WV_GPUTASK_CREATE_TEXTURE,
		WV_GPUTASK_BUFFER_TEXTURE_DATA,
		WV_GPUTASK_DESTROY_TEXTURE,
		WV_GPUTASK_BIND_TEXTURE,

		WV_GPUTASK_DRAW
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cCommandBuffer
	{
	public:
		cCommandBuffer( const size_t& _initialSize )
		{ 
			m_buffer.allocate( _initialSize );
		}

		void flush()
		{
			m_buffer.clear();
			m_numCommands = 0;

			callback.bind( nullptr );
			callbacker = nullptr;
		}

		template<typename T> 
		void push( const eGPUTaskType& _type, T* _pInfo );
		void push( const eGPUTaskType& _type ) { push<std::nullptr_t>( _type, nullptr ); }

		wv::cMemoryStream& getBuffer() { return m_buffer; }
		size_t numCommands() { return m_numCommands; }
		
		wv::Function<void, void*> callback;
		void* callbacker = nullptr;

///////////////////////////////////////////////////////////////////////////////////////

	private:
		wv::cMemoryStream m_buffer;
		size_t m_numCommands = 0;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline void cCommandBuffer::push( const eGPUTaskType& _type, T* _pInfo )
	{
		m_numCommands++;
		m_buffer.push( _type );
		m_buffer.push( *_pInfo, sizeof( T ) );
	}
	
}

