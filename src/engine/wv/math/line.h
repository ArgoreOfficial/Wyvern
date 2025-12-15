#pragma once

#include "vector3.h"

namespace wv {

template<typename Ty>
class Line3
{
public:
	Line3() : 
		a{ 0.0f, 0.0f, 0.0f },
		b{ 0.0f, 0.0f, 0.0f }
	{};

	Line3( const wv::Vector3<Ty>& _start, const wv::Vector3<Ty>& _end ) : 
		a{ _start },
		b{ _end }
	{ }

	Line3( const wv::Line3<Ty>& _line, const wv::Vector3<Ty>& _end ) : 
		a{ _line.b }, 
		b{ _end }
	{ }

	Line3( const wv::Vector3<Ty>& _start, const wv::Line3<Ty>& _line ) : 
		a{ _start },
		b{ _line.a } 
	{ }

	wv::Vector3<Ty> a{};
	wv::Vector3<Ty> b{};

};

typedef Line3<float> Line3f;

}