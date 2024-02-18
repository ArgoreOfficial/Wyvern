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

void wv::cFramebuffer::create()
{
	m_framebuffer_object = cRenderer::getInstance().getBackend()->createFramebuffer();
}

void wv::cFramebuffer::finalize()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	cm::cWindow* window = cApplication::getInstance().getWindow();

	backend->attachFramebuffer( m_framebuffer_object );

	// depth buffer, make optional?
	backend->addFramebufferRenderbuffer( m_framebuffer_object, cm::eRenderbufferType::RenderbufferType_Depth, window->getWidth(), window->getHeight() );

}

void wv::cFramebuffer::addTexture( std::string _name, cm::eTextureFormat _format, cm::eTextureType _type )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	cm::cWindow* window = cApplication::getInstance().getWindow();

	backend->addFramebufferTexture( m_framebuffer_object, _name, _format, _type, window->getWidth(), window->getHeight() );
}

void wv::cFramebuffer::bind()
{
	cRenderer::getInstance().getBackend()->bindFramebuffer( &m_framebuffer_object );
}

void wv::cFramebuffer::bindTextures( cShader* _shader )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	for ( int i = 0; i < m_framebuffer_object.textures.size(); i++ )
	{
		int shader_loc = _shader->getUniformLocation( m_framebuffer_object.textures[ i ].name );
		backend->setUniformInt( shader_loc, i );
		backend->setActiveTextureSlot( i );
		backend->bindTexture2D( m_framebuffer_object.textures[ i ].handle );
	}
}

void wv::cFramebuffer::unbind()
{
	cRenderer::getInstance().getBackend()->bindFramebuffer( 0 );
}

void wv::cFramebuffer::onResize()
{
	// TODO: this
}
