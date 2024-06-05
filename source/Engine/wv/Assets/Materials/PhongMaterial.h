#pragma once

#include "IMaterial.h"

namespace wv
{
	class Texture;
	class GraphicsDevice;

	struct PhongMaterialDesc
	{
		const char* albedoTexturePath = "res/textures/throbber.gif";
	};

	class PhongMaterial : public IMaterial
	{
	public:
		PhongMaterial( PhongMaterialDesc _desc = {} ) : desc{ _desc } { }
		virtual void create( GraphicsDevice* _device, const char* _vs = nullptr, const char* _fs = nullptr ) override;
		virtual void destroy( GraphicsDevice* _device ) override;

		virtual void materialCallback() override;
		virtual void instanceCallback( Node* _instance ) override;

	private:
		PhongMaterialDesc desc;
		Texture* m_albedoTexture = nullptr;

	};
}