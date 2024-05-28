#pragma once

#include <string>
#include <unordered_map>

#include <wv/Types.h>

namespace wv
{
	enum UniformBlockSubmitMode
	{
		WV_UNIFORM_BLOCK_SUBMIT_IMMEDIATE,
		WV_UNIFORM_BLOCK_SUBMIT_DEFERRED
	};

	struct Uniform
	{
		unsigned int index;
		unsigned int offset;
		std::string name;
	};

	struct UniformBlockDesc
	{
		const char* name;

		const char** uniforms;
		int numUniforms;
	};

	class UniformBlock
	{
	public:
		friend class GraphicsDevice;

		UniformBlock() { }
		
		template<typename T>
		void set( std::string _name, T _data );

	private:

		wv::Handle m_index;
		wv::Handle m_bufferHandle;
		wv::Handle m_bindingIndex;

		char* m_buffer;
		int m_bufferSize;

		std::unordered_map<std::string, Uniform> m_uniforms;

	};

	typedef std::unordered_map<std::string, wv::UniformBlock> UniformBlockMap;



	template<typename T>
	inline void UniformBlock::set( std::string _name, T _data )
	{
		unsigned int offset = m_uniforms[ _name ].offset;
		memcpy( m_buffer + offset, &_data, sizeof( T ) );
	}
}