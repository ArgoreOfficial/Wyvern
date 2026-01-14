#include "renderer.h"

#include <wv/debug/log.h>

#include <wv/entity/world.h>
#include <wv/graphics/viewport.h>

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initialize()
{
	return true;
}

void wv::Renderer::shutdown()
{
}

void wv::Renderer::prepare( uint32_t _width, uint32_t _height )
{
}

void wv::Renderer::finalize()
{
}
