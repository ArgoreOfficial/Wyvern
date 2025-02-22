#pragma once

#include <wv/Graphics/Texture.h>
#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cTextureResource : public iResource
	{
	public:
		
		cTextureResource( const std::string& _name = "", const std::string& _path = "", eTextureFiltering _filtering = WV_TEXTURE_FILTER_NEAREST ) :
			iResource{ _name, _path },
			m_filtering{ _filtering }
		{ }

		void load  ( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics ) override;
		void unload( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics ) override;

		TextureID m_textureID;
	private:

		eTextureFiltering m_filtering;
		uint8_t* m_pData = nullptr;
		size_t m_dataSize = 0;
	};

}
