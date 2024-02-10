#include "cApplication.h"

#include <cm/Core/cWindow.h>
#include <cm/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <cm/Framework/cVertexLayout.h>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

#define STB_IMAGE_IMPLEMENTATION
#include <cm/Core/stb_image.h>

#include <iostream>
#include <fstream>
#include <string>

cApplication::cApplication() :
	m_window{ new cWindow() },
	m_renderer{ new cRenderer() }
{
}

cApplication::~cApplication()
{
	m_window->destroy();
}

void cApplication::onCreate()
{
	m_window->create( 1500, 1000, "renderer idfk" );
	m_renderer->create( *m_window, cRenderer::eBackendType::OpenGL );

}

void cApplication::onResize( int _width, int _height )
{
	m_renderer->onResize( _width, _height );
	m_window->onResize( _width, _height );
}

void cApplication::onRawInput( sInputInfo* _info )
{

}

struct sVertex
{
	wv::cVector3f position;
	wv::cVector2f UV;
};

void cApplication::run()
{
	double time = m_window->getTime();;
	double delta_time = 0.0;
	iBackend* backend = m_renderer->getBackend();


	/* sandbox */

	sVertex points[] = {
		/*    Position            UV      */
		{ { -0.5,  0.5, 0}, { 0.0f, 0.0f } },
		{ { -0.5, -0.5, 0}, { 0.0f, 1.0f } },
		{ {  0.5, -0.5, 0}, { 1.0f, 1.0f } },
		{ {  0.5,  0.5, 0}, { 1.0f, 0.0f } },
	};

	unsigned int indices[] = {
		0,1,2,
		0,2,3
	};

	std::string vert = loadShaderSource( "../res/texture.vert" );
	std::string frag = loadShaderSource( "../res/texture.frag" );
	sShader vert_shader = m_renderer->createShader( vert.data(), eShaderType::Shader_Vertex );
	sShader frag_shader = m_renderer->createShader( frag.data(), eShaderType::Shader_Fragment );

	hShaderProgram shader = backend->createShaderProgram();
	backend->attachShader( shader, vert_shader );
	backend->attachShader( shader, frag_shader );
	backend->linkShaderProgram( shader );

	/* create vertex array */
	hVertexArray vertex_array = backend->createVertexArray();
	backend->bindVertexArray( vertex_array );

	/* create vertex buffer */
	sBuffer vertex_buffer = backend->createBuffer( eBufferType::Buffer_Vertex );
	backend->bufferData( vertex_buffer, points, sizeof( points ) );

	sBuffer index_buffer = backend->createBuffer( eBufferType::Buffer_Index );
	backend->bufferData( index_buffer, indices, sizeof( indices ) );

	cVertexLayout layout;
	layout.push<float>( 3 );
	layout.push<float>( 2 );
	
	backend->bindVertexLayout( layout );


	/* texture */

	sTexture2D texture = backend->createTexture();
	unsigned char* data = stbi_load( "../res/wyvern_logo_white.png", &texture.width, &texture.height, &texture.num_channels, 0 );
	backend->generateTexture( texture, data );
	stbi_image_free( data );

	/* sandbox */

	while ( !m_window->shouldClose() )
	{
		double now = m_window->getTime();
		delta_time = now - time;
		time = now;
		
		m_window->beginFrame();
		m_renderer->beginFrame();
		m_renderer->clear( 0x000000FF );

		backend->useShaderProgram( shader );

		backend->bindVertexArray( vertex_array );
		backend->bindTexture2D( texture.handle );

		backend->drawElements( 6, eDrawMode::DrawMode_Triangle );


		m_renderer->endFrame();
		m_window->endFrame();
	}
}

std::string cApplication::loadShaderSource( const char* _path )
{
	std::string line, text;
	std::ifstream in( _path );

	while ( std::getline( in, line ) )
	{
		text += line + "\n";
	}
	
	return text;
}

