#pragma once

#include "IMaterial.h"

namespace wv
{
	class Texture;
	
	class PhongMaterial : public IMaterial
	{
	public:
		PhongMaterial() { }
		virtual void create( GraphicsDevice* _device ) override { };
	private:

	};
}