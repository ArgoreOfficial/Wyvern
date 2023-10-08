#pragma once
#include "GLHeader.h"
#include <string>
#include <vendor/stb_image/stb_image.h>

namespace WV
{
	class Texture
	{
	private:
		unsigned int _renderID;
		std::string _filePath;
		unsigned char* _localBuffer;
		int _width, _height, _bpp;

	public:
		Texture( std::string path );
		~Texture();

		void Bind( unsigned int slot = 0 ) const;
		void Unbind() const;
	};
}