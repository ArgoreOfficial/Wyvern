#pragma once
#include "GLHeader.h"
#include <string>
#include <stb_image.h>

class Texture
{
private:
	unsigned int _renderID;
	std::string _filePath;
	unsigned char* _localBuffer;
	int _width, _height, _bpp;

public:
	Texture(std::string path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;
};