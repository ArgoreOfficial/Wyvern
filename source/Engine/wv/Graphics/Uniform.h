#pragma once

#include <wv/Types.h>

#include <wv/Debug/Print.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct sUniform
	{
		unsigned int index = 0;
		unsigned int offset = 0;
		std::string name;
	};

///////////////////////////////////////////////////////////////////////////////////////

}