#include "GraphicsDevice.h"

#include <glad/glad.h>

#include <stdio.h>

wv::GraphicsDevice::GraphicsDevice( GraphicsDeviceDesc* _desc )
{
	if ( !gladLoadGLLoader( _desc->loadProc ) )
		fprintf( stderr, "Failed to initialize GLAD\n" );
	
	// glViewport( 0, 0, _window.getWidth(), _window.getHeight() );

	printf( "Lindorm Renderer Intialized\n" );
}

void wv::GraphicsDevice::setRenderTarget( DummyRenderTarget* _target )
{
	glBindFramebuffer( GL_FRAMEBUFFER, _target->framebuffer ); /// TEMPORARY
	glViewport( 0, 0, _target->width, _target->height );
}

void wv::GraphicsDevice::clearRenderTarget( const float _color[ 4 ] )
{
	glClearColor( _color[ 0 ], _color[ 1 ], _color[ 2 ], _color[ 3 ] );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
