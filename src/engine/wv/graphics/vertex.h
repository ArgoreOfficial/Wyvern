#pragma once

#include <wv/types.h>

namespace wv {

struct Vertex
{
	Vertex() = default;
	Vertex( float _x, float _y, float _u, float _v ) {
		position[ 0 ] = _x;
		position[ 1 ] = _y;

		uvs[ 0 ] = _u;
		uvs[ 1 ] = _v;
	}

	float position[ 2 ] = { 0.0f, 0.0f };
	float uvs[ 2 ] = { 0.0f, 0.0f };

};

struct GLVertexBuffer
{
	wv::GLHandle storage_buffer_handle = 0;
	size_t size = 0;
};

}