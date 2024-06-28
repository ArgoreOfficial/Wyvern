#pragma once

#include <wv/Debug/Print.h>
#include <wv/Types.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	enum UniformBlockSubmitMode
	{
		WV_UNIFORM_BLOCK_SUBMIT_IMMEDIATE,
		WV_UNIFORM_BLOCK_SUBMIT_DEFERRED
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct Uniform
	{
		unsigned int index = 0;
		unsigned int offset = 0;
		std::string name;
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct UniformBlockDesc
	{
		std::string name;
		std::vector<Uniform> uniforms;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class UniformBlock
	{

	public:

		friend class iGraphicsDevice;

		UniformBlock() { }
		
		template<typename T> void set( const std::string& _name, T _data ) { set<T>( _name, &_data ); }
		template<typename T> void set( const std::string& _name, T* _data );

///////////////////////////////////////////////////////////////////////////////////////

	private:

		wv::Handle m_index = 0;
		wv::Handle m_bufferHandle = 0;
		wv::Handle m_bindingIndex = 0;

		char* m_buffer = nullptr;
		int m_bufferSize = 0;

		std::unordered_map<std::string, Uniform> m_uniforms;

	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef std::unordered_map<std::string, wv::UniformBlock> UniformBlockMap;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline void UniformBlock::set( const std::string& _name, T* _data )
	{
		if ( m_uniforms.count( _name ) == 0 )
		{
			Debug::Print( Debug::WV_PRINT_WARN, "Uniform %s does not exist\n", _name.c_str() );
			return;
		}
		unsigned int offset = m_uniforms[ _name ].offset;
		memcpy( m_buffer + offset, _data, sizeof( T ) );
	}

}