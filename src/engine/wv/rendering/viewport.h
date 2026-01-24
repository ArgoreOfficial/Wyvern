#pragma once

#include <wv/math/vector2.h>

namespace wv {

class ViewVolume;

class Viewport
{
public:
	Viewport() = default;

	inline double getAspect() const { return (double)m_size.x / (double)m_size.y; }

	void setSize( int _width, int _height ) { m_size = { _width, _height }; }
	void setViewVolume( ViewVolume* _camera ) { m_viewVolume = _camera; }

	wv::Vector2i    getSize()       const { return m_size; }
	wv::ViewVolume* getViewVolume() const { return m_viewVolume; }

private:

	wv::Vector2i m_size{ 900, 600 };
	ViewVolume* m_viewVolume = nullptr;
};

}