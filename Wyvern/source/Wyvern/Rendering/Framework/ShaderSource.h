#pragma once
#include <string>
#include <fstream>

namespace WV
{
	class ShaderSource
	{
	public:
		std::string vertexSource;
		std::string fragmentSource;

		ShaderSource( std::string path );
		~ShaderSource();
	};
}