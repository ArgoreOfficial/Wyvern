#pragma once
#include <Wyvern/API/Core.h>
#include <string>
#include <fstream>

class WYVERN_API ShaderSource
{
public:
	std::string vertexSource;
	std::string fragmentSource;

	ShaderSource( std::string path );
	~ShaderSource();
};