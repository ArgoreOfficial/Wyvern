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

		void load  ( FileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) override;
		void unload( FileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) override;

		size_t getDataSize() {
			return m_width * m_height * m_numChannels;
		}

		TextureID m_textureID;
	private:

		TextureFiltering m_filtering;
		uint8_t* m_pData = nullptr;
		
		int m_width = 0;
		int m_height = 0;
		int m_numChannels = 0;
	};

}
