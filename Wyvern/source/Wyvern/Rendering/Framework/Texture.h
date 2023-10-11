#pragma once
#include "GLHeader.h"
#include <string>
#include <Wyvern/Assets/TextureAsset.h>

namespace WV
{
	class Texture
	{
	public:
		Texture( Asset::TextureAssetData _data );
		~Texture();

		unsigned int getID() { return m_renderID; }
		void Bind( unsigned int slot = 0 ) const;
		void Unbind() const;
	
	private:
		unsigned int m_renderID;

	};
}