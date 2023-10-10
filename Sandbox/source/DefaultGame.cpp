#include "DefaultGame.h"
#include <Wyvern/Managers/AssetManager.h>
#include <Wyvern/Rendering/Renderer.h>

void DefaultGame::load()
{
	m_camera = new WV::Camera(40, 800, 800);

	WV::AssetManager::loadModel( &m_testModel, "meshes/fire.obj", "textures/whisperingdeath.png" );
	WV::AssetManager::loadModel( &m_testModel2, "meshes/fire.obj", "textures/stone_card.png" );


	m_camera->rotate( glm::vec3( 0.0f, 0.0f, 0.0f ) );
	m_camera->move( glm::vec3( 0, 0.0f, 3.0f ) );

	WV::Renderer::setActiveCamera( m_camera );
}

void DefaultGame::update( float _deltaTime )
{
	float posy = sinf( WV::Application::getTime() * 2.0f );
	m_testModel->setPosition( { -0.5f, posy / 15.0f, 0 } );
	m_testModel->rotate( { 0, _deltaTime * 45, 0 } );

	m_testModel2->setPosition( { 0.5f, posy / 15.0f, 0 } );
	m_testModel2->rotate( { 0, _deltaTime * 45, 0 } );
}

void DefaultGame::draw()
{
	WV::Renderer::getInstance().draw( *m_testModel );
	WV::Renderer::getInstance().draw( *m_testModel2 );
}