#pragma once

#include "IMaterial.h"

namespace wv
{
	class Texture;
	class GraphicsDevice;

	class UnlitMaterial : public IMaterial
	{
	public:
		UnlitMaterial() { }
		virtual void create( GraphicsDevice* _device, const char* _vs = nullptr, const char* _fs = nullptr ) override;
		virtual void destroy( GraphicsDevice* _device ) override;

		virtual void materialCallback() override;
		virtual void instanceCallback( Node* _instance ) override;

	private:
		Texture* m_albedoTexture = nullptr;

	};
}