#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Types.h>
#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	enum TextureChannels
	{
		WV_TEXTURE_CHANNELS_R = 1,
		WV_TEXTURE_CHANNELS_RG,
		WV_TEXTURE_CHANNELS_RGB,
		WV_TEXTURE_CHANNELS_RGBA
	};

	enum eTextureFormat
	{
		WV_TEXTURE_FORMAT_BYTE,
		WV_TEXTURE_FORMAT_INT,
		WV_TEXTURE_FORMAT_FLOAT
	};

	enum eTextureFiltering
	{
		WV_TEXTURE_FILTER_NEAREST,
		WV_TEXTURE_FILTER_LINEAR,
	};

	class cTextureResource;

	struct sTextureDesc
	{
		TextureChannels   channels  = WV_TEXTURE_CHANNELS_RGB;
		eTextureFormat    format    = WV_TEXTURE_FORMAT_BYTE;
		eTextureFiltering filtering = WV_TEXTURE_FILTER_NEAREST;
		int width = 0;
		int height = 0;
		int numChannels = 0;
		bool generateMipMaps = false;
	};

	struct sTexture
	{
		eTextureFiltering m_filtering;

		wv::Handle textureObjectHandle = 0; // opengl specific
		uint64_t textureHandle = 0; // opengl specific
		
		uint8_t* pData = nullptr;
		unsigned int dataSize = 0;

		int width  = 0;
		int height = 0;
		int numChannels = 0;

		void* pPlatformData = nullptr;
	};

	class cTextureResource : public iResource
	{
	public:
		
		cTextureResource( const std::string& _name = "", const std::string& _path = "", eTextureFiltering _filtering = WV_TEXTURE_FILTER_NEAREST ) :
			iResource{ _name, _path },
			m_filtering{ _filtering }
		{ }

		void load  ( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;
		void unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;

		TextureID m_textureID;
	private:

		eTextureFiltering m_filtering;
		uint8_t* m_pData = nullptr;
		size_t m_dataSize = 0;
	};

}
