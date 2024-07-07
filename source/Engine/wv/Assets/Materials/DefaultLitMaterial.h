#pragma once

#include <wv/Assets/Materials/Material.h>

namespace wv
{
	class cPhongMaterial : public iMaterial
	{
	public:

		cPhongMaterial( cShaderProgram* _program ) :
			iMaterial{ "phong", _program }
		{ }


		virtual void setMaterialUniforms() override;
		virtual void setInstanceUniforms( Mesh* _instance ) override;

	};
}