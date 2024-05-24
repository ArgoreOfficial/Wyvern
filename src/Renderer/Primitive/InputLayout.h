#pragma once

#include "../../types.h"

namespace lo
{
	struct sInputLayoutElement
	{
		size_t    num;
		lo::eType type;
		bool      normalized;
		size_t    stride;
	};

	struct sInputLayoutInfo
	{
		sInputLayoutElement* elements = nullptr;
		size_t num_elements = 0;
	};
}