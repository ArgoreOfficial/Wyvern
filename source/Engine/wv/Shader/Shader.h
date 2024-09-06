#pragma once

#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct Memory;

	enum eShaderType
	{
		WV_SHADER_TYPE_VERTEX,
		WV_SHADER_TYPE_FRAGMENT
	};

	struct sShaderSource
	{
		Memory* data;
	};

	struct sShader
	{
		Handle handle;

		eShaderType type;
		sShaderSource source;

		PlatformData pPlatformData;
	};

}