#include "cApplication.h"

#include <cm/Core/cWindow.h>
#include <cm/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <wv/Graphics/cSprite.h>

#include <wv/Manager/cContentManager.h>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

#define STB_IMAGE_IMPLEMENTATION
#include <cm/Core/stb_image.h>

cApplication::cApplication() :
	m_window  { new cm::cWindow() },
	m_renderer{ new cm::cRenderer() }
{
}

cApplication::~cApplication()
{
	m_window->destroy();
}

void cApplication::onCreate()
{
	m_window->create( 1500, 1000, "renderer idfk" );
	m_renderer->create( *m_window, cm::cRenderer::eBackendType::OpenGL );

	wv::cContentManager::getInstance();
}

void cApplication::onResize( int _width, int _height )
{
	m_renderer->onResize( _width, _height );
	m_window->onResize( _width, _height );
}

void cApplication::onRawInput( sInputInfo* _info )
{

}

void cApplication::run()
{
	double time = m_window->getTime();;
	double delta_time = 0.0;
	cm::iBackend* backend = m_renderer->getBackend();
	wv::cContentManager& content_manager = wv::cContentManager::getInstance();

	/* sandbox */

	wv::cSprite sprite;
	sprite.create( "../res/textures/wyvern_logo_white.png" );

	/* sandbox */

	while ( !m_window->shouldClose() )
	{
		double now = m_window->getTime();
		delta_time = now - time;
		time = now;
		
		m_window->beginFrame();
		m_renderer->beginFrame();
		m_renderer->clear( 0x000000FF );

		sprite.render();

		m_renderer->endFrame();
		m_window->endFrame();
	}
}

