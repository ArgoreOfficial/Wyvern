#include "cFramebuffer.h"

#include <cm/Backends/iBackend.h>
#include <cm/Core/cWindow.h>

#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>
#include <wv/Graphics/cShader.h>

wv::cFramebuffer::cFramebuffer( void )
{

}

wv::cFramebuffer::~cFramebuffer( void )
{

}

void wv::cFramebuffer::create( int _samples )
{
	//m_framebuffer_object = cRenderer::getInstance().getBackend()->createFramebuffer();

	//m_framebuffer_object.samples = _samples;
}

void wv::cFramebuffer::finalize()
{
	//cRenderer::getInstance().getBackend()->attachFramebuffer( m_framebuffer_object );
}

void wv::cFramebuffer::addTexture( std::string _name, cm::eTextureFormat _format, cm::eTextureType _type )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	
	m_size.x = 300;
	m_size.y = 300;

	cm::eTextureTarget target = cm::TextureTarget_Texture2D;
	if ( m_framebuffer_object.samples != 0 ) target = cm::TextureTarget_Texture2DMultisample;

	//cm::sTexture2D texture = backend->createTexture( _format, target, _type, m_size.x, m_size.y );
	//backend->addFramebufferTexture( m_framebuffer_object, _name, texture );
}

void wv::cFramebuffer::addRenderbuffer( std::string _name, cm::eRenderbufferType _type )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	
	//backend->addFramebufferRenderbuffer( m_framebuffer_object, cm::eRenderbufferType::RenderbufferType_Depth, m_size.x, m_size.y );
}

void wv::cFramebuffer::bind()
{
	//cRenderer::getInstance().getBackend()->bindFramebuffer( m_framebuffer_object );
}

void wv::cFramebuffer::bindTexturesToShader( cShader* _shader )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	for ( int i = 0; i < m_framebuffer_object.textures.size(); i++ )
	{
		int shader_loc = _shader->getUniformLocation( m_framebuffer_object.textures[ i ].name );
		//backend->setUniformInt( shader_loc, i );
		//backend->setActiveTextureSlot( i );
		//backend->bindTexture2D( m_framebuffer_object.textures[ i ] );
	}
}

void wv::cFramebuffer::unbind()
{
	//cRenderer::getInstance().getBackend()->unbindFramebuffer();
}

void wv::cFramebuffer::onResize( int _width, int _height )
{
	if ( m_size.x == _width && m_size.y == _height ) // no need to resize
		return;

	m_size.x = _width;
	m_size.y = _height;

	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	
	std::vector<cm::sTexture2D>    textures = m_framebuffer_object.textures;
	std::vector<cm::sRenderbuffer> renderbuffers = m_framebuffer_object.renderbuffers;

	/*cRenderer::getInstance().getBackend()->destroyFramebuffer( m_framebuffer_object );
	m_framebuffer_object = cRenderer::getInstance().getBackend()->createFramebuffer();

	for ( int i = 0; i < textures.size(); i++ )
	{
		cm::sTexture2D texture = backend->createTexture( textures[ i ].format, textures[ i ].target, textures[i].type, _width, _height);
		backend->addFramebufferTexture( m_framebuffer_object, textures[ i ].name, texture );
	}
	
	for ( int i = 0; i < renderbuffers.size(); i++ )
		backend->addFramebufferRenderbuffer( m_framebuffer_object, renderbuffers[ i ].type, _width, _height );*/

}
