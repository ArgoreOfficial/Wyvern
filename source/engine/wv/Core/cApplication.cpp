#include "cApplication.h"

#include <cm/Core/cWindow.h>
#include <cm/Backends/iBackend.h>

#include <wv/Core/cRenderer.h>
#include <wv/Graphics/cSprite.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>
#include <wv/Manager/cContentManager.h>

#define STB_IMAGE_IMPLEMENTATION
#include <cm/Core/stb_image.h>

wv::cApplication::cApplication() :
	m_window  { new cm::cWindow() }
{
}

wv::cApplication::~cApplication()
{
	m_window->destroy();
}

void wv::cApplication::create()
{
	m_window->create( 1500, 1000, "renderer idfk" );
	wv::cRenderer::getInstance().create();

	wv::cContentManager::getInstance();
}

void wv::cApplication::onResize( int _width, int _height )
{
	wv::cRenderer::getInstance().onResize( _width, _height );
	m_window->onResize( _width, _height );
}

void wv::cApplication::onRawInput( sInputInfo* _info )
{
	// change to event manager?
}

void wv::cApplication::run()
{
	double time = m_window->getTime();;
	double delta_time = 0.0;
	

	/* sandbox */
	wv::cSprite sprite;
	sprite.create( "../res/textures/wyvern_logo.png" );

	wv::cSprite sprite2;
	sprite2.create( "../res/textures/funnyman.png" );
	sprite.getTransform().rotate( { 0.1f, 0.0f, 0.0f } );

	/* sandbox */

	wv::cRenderer& renderer = wv::cRenderer::getInstance();

	while ( !m_window->shouldClose() )
	{
		updateDeltaTime( time, delta_time );

		m_window->processInput();

		// scenemanager.update();

		sprite.getTransform().rotate( { 1.0f * (float)delta_time, 2.0f * (float)delta_time, 3.0f * (float)delta_time } );

		renderer.begin();
		renderer.clear( 0xFFFFFFFF );

		sprite2.render();
		sprite.render();

		renderer.end();
		m_window->display();
	}
}

void wv::cApplication::updateDeltaTime( double& _time, double& _delta_time )
{
	double now = m_window->getTime();
	_delta_time = now - _time;
	_time = now;
}

