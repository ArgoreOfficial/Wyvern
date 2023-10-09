#include "DefaultGame.h"
#include <Wyvern/Application.h>

void DefaultGame::load()
{
	m_camera = new WV::Camera(40, 800, 800);
	m_testModel = WV::RenderObject::createFromFiles( "meshes/NormalWhisperingDeath.obj", "shaders/default.shader", "textures/whisperingdeath.png" );
	
	m_camera->move( glm::vec3( 0, 0.5f, 2.0f ) );
	m_camera->rotate( glm::vec3( 20.0f, 0.0f, 0.0f ) );

	WV::Renderer::setActiveCamera( m_camera );
}

void DefaultGame::update( float _deltaTime )
{
	float posy = sinf( WV::Application::getTime() * 2.0f );
	m_testModel->setPosition( { 0, posy / 15.0f, 0 } );
	m_testModel->rotate( { 0, _deltaTime * 45, 0 } );
}

void DefaultGame::draw()
{
	WV::Renderer::getInstance().draw( *m_testModel );
}