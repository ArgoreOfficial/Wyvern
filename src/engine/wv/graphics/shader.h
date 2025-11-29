#pragma once

#include <wv/types.h>

namespace wv {

struct GLShaderPipeline
{
	wv::GLHandle vert_module_handle = 0;
	wv::GLHandle frag_module_handle = 0;
	wv::GLHandle pipeline_handle = 0;
};

}