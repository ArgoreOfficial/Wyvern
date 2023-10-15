#pragma once
#include <string>
#include <fstream>

namespace WV
{
	class ShaderSource
	{
	public:
		std::string m_vertexSource;
		std::string m_fragmentSource;
		std::string m_path;

		ShaderSource( std::string path );
		~ShaderSource();
	};
}