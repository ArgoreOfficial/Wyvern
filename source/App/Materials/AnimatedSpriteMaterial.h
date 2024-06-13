#pragma once

#include <wv/Assets/Materials/Material.h>
#include <wv/Math/Vector2.h>

class AnimatedSpriteMaterial : public wv::Material
{
public:

	virtual void destroy();

	virtual void materialCallback();
	virtual void instanceCallback( wv::Mesh* _instance ) override;

	void setFrame( int _frame, int _numFramesX, int _numFramesY );

private:

	wv::Vector2f m_uvOffset;
};