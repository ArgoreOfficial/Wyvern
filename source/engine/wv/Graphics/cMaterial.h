#pragma once

#include <wv/Graphics/cShader.h>
#include <cm/Framework/Texture.h>

#include <wv/Math/Vector4.h>

#include <map>
#include <string>

namespace wv
{
	class cMaterial
	{
	public:
		 cMaterial();
		~cMaterial();

		void bind();
		void unbind();

		cm::sTexture2D* addTexture( const std::string& _name, std::string _path );
		cm::sTexture2D* getTexture( const std::string& _name );

		wv::cShader* shader = nullptr;

	private:

		std::map<std::string, cm::sTexture2D*> m_textures;
		std::map<std::string, int> m_bindings;
		
	};
}