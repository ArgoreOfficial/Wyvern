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
	//backend->addFramebufferRenderbuffer( m_framebuffer_object, cm::eRenderbufferType::RenderbufferType_Depth, window->getWidth(), window->getHeight() );

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

void wv::cFramebuffer::bindTexturesToShader( cShader* _shader )
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
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	cm::cWindow* window = cApplication::getInstance().getWindow();

	std::vector<cm::sTexture2D> textures = m_framebuffer_object.textures;
	std::vector<cm::sRenderbuffer> renderbuffers = m_framebuffer_object.renderbuffers;

	cRenderer::getInstance().getBackend()->destroyFramebuffer( m_framebuffer_object );
	m_framebuffer_object = cRenderer::getInstance().getBackend()->createFramebuffer();

	int width = window->getWidth();
	int height = window->getHeight();

	printf( "resized framebuffer %i,%i\n", width, height );

	for ( int i = 0; i < textures.size(); i++ )
		backend->addFramebufferTexture( m_framebuffer_object, textures[ i ].name, textures[ i ].format, textures[ i ].type, width, height );
	
	backend->addFramebufferRenderbuffer( m_framebuffer_object, cm::RenderbufferType_Depth, width, height );
}
