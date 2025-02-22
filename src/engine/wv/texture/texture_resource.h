#pragma once

#include <wv/graphics/Texture.h>
#include <wv/resource/resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class TextureResource : public IResource
	{
	public:
		
		TextureResource( const std::string& _name = "", const std::string& _path = "", TextureFiltering _filtering = WV_TEXTURE_FILTER_NEAREST ) :
			IResource{ _name, _path },
			m_filtering{ _filtering }
		{ }

		void load  ( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics ) override;
		void unload( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics ) override;

		TextureID m_textureID;
	private:

		TextureFiltering m_filtering;
		uint8_t* m_pData = nullptr;
		size_t m_dataSize = 0;
	};

}
