#pragma once

#include <wv/Types.h>
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

	enum TextureFormat
	{
		WV_TEXTURE_FORMAT_BYTE,
		WV_TEXTURE_FORMAT_INT,
		WV_TEXTURE_FORMAT_FLOAT
	};

	enum TextureFiltering
	{
		WV_TEXTURE_FILTER_NEAREST,
		WV_TEXTURE_FILTER_LINEAR,
	};

	struct TextureDesc
	{			 
		TextureChannels channels = WV_TEXTURE_CHANNELS_RGB;
		TextureFormat format = WV_TEXTURE_FORMAT_BYTE;
		TextureFiltering filtering = WV_TEXTURE_FILTER_NEAREST;
		int width = 0;
		int height = 0;
		bool generateMipMaps = false;
	};

	class Texture : public iResource
	{
	public:
		
		Texture( const std::string& _name = "", const std::wstring& _path = L"" ) :
			iResource{ _name, _path } 
		{ }

		void load  ( cFileSystem* _pFileSystem ) override;
		void unload( cFileSystem* _pFileSystem ) override;

		void create ( iGraphicsDevice* _pGraphicsDevice ) override;
		void destroy( iGraphicsDevice* _pGraphicsDevice ) override;

		void setWidth ( int _width )  { m_width = _width; }
		void setHeight( int _height ) { m_height = _height; }

		int getWidth ( void ) { return m_width; }
		int getHeight( void ) { return m_height; }
		int getNumChannels( void ) { return m_numChannels; }

		uint8_t*     getData    ( void ) { return m_pData; }
		unsigned int getDataSize( void ) { return m_dataSize; }

	private:
		uint8_t* m_pData = nullptr;
		unsigned int m_dataSize = 0;

		int m_width  = 0;
		int m_height = 0;
		int m_numChannels = 0;
	};

}
