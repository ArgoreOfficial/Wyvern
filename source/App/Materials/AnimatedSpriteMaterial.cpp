#include "AnimatedSpriteMaterial.h"

#include <wv/Application/Application.h>
#include <wv/Assets/Texture.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Math/Transform.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Mesh.h>

void AnimatedSpriteMaterial::destroy()
{
	Material::destroy();
}

void AnimatedSpriteMaterial::materialCallback()
{
	Material::materialCallback();
}

void AnimatedSpriteMaterial::instanceCallback( wv::Mesh* _instance )
{
	Material::instanceCallback( _instance );

	wv::UniformBlock& instanceBlock = m_pipeline->uniformBlocks[ "UbInstanceData" ];

	instanceBlock.set( "u_UVOffset", m_uvOffset );
}

void AnimatedSpriteMaterial::setFrame( int _frame, int _numFramesX, int _numFramesY )
{
	int width = m_textures[ 0 ]->width;
	int height = m_textures[ 0 ]->height;

	float fw = width  / (float)_numFramesX;
	float fh = height / (float)_numFramesY;
	float uSize = fw / width;
	float vSize = fh / height;

	m_uvOffset.x = uSize * _frame;
}
