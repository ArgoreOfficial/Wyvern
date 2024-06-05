#pragma once

#include "IMaterial.h"

namespace wv
{
	class Texture;
	class GraphicsDevice;

	class PhongMaterial : public IMaterial
	{
	public:
		PhongMaterial() { }
		virtual void create( GraphicsDevice* _device ) override;
		virtual void destroy( GraphicsDevice* _device ) override;

		virtual void materialCallback() override;
		virtual void instanceCallback( Node* _instance ) override;

	private:
		Texture* m_albedoTexture = nullptr;

	};
}