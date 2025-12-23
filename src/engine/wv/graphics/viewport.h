#pragma once

#include <wv/math/vector2.h>

namespace wv {

class ICamera;

class Viewport
{
public:
	Viewport() = default;

	inline double getAspect() const { return (double)m_size.x / (double)m_size.y; }

	void setSize( int _width, int _height ) { m_size = { _width, _height }; }
	void setCamera( ICamera* _camera ) { m_camera = _camera; }

	wv::Vector2i getSize  () const { return m_size; }
	wv::ICamera* getCamera() const { return m_camera; }

private:

	wv::Vector2i m_size{ 900, 600 };
	ICamera* m_camera = nullptr;
};

}