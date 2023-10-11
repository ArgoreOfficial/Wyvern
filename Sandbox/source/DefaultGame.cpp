#include "DefaultGame.h"
#include <Wyvern/Managers/AssetManager.h>
#include <Wyvern/Rendering/Renderer.h>

void DefaultGame::load()
{

	WV::AssetManager::queueLoad<WV::Mesh>( "meshes/logo.obj" );
	WV::AssetManager::queueLoad<WV::Texture>( "textures/logo_fire_BaseColor.png" );

	// WV::AssetManager::loadModel( &m_testModel, "meshes/logo.obj", "textures/logo_fire_BaseColor.png" );
	// WV::AssetManager::loadModel( &m_testModel2, "meshes/logo.obj", "textures/logo_fire_Roughness.png" )
}

void DefaultGame::start()
{
	m_camera = new WV::Camera(40, 800, 800);
	
	m_camera->rotate( glm::vec3( 0.0f, 0.0f, 0.0f ) );
	m_camera->move( glm::vec3( 0, 0.0f, 3.0f ) );

	m_testModel = WV::AssetManager::assembleModel( "logo.obj", "logo_fire_BaseColor.png" );

	WV::Renderer::setActiveCamera( m_camera );
}

void DefaultGame::update( float _deltaTime )
{
	float posy = sinf( WV::Application::getTime() * 2.0f );
	m_testModel->setPosition( { -0.5f, posy / 15.0f, 0 } );
	m_testModel->rotate( { 0, _deltaTime * 45, 0 } );

	// m_testModel2->setPosition( { 0.5f, posy / 15.0f, 0 } );
	// m_testModel2->rotate( { 0, _deltaTime * 45, 0 } );
}

void DefaultGame::draw()
{
	WV::Renderer::getInstance().draw( *m_testModel );
	// WV::Renderer::getInstance().draw( *m_testModel2 );
}