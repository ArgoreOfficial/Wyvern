#include "SceneGame.h"

#include <wv/Application/Application.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/ModelParser.h>
#include <wv/Primitive/Mesh.h>

#include <glad/glad.h>

SceneGame::SceneGame()
{

}

SceneGame::~SceneGame()
{

}

void SceneGame::onLoad()
{

	wv::assimp::Parser parser;
	m_player = parser.load( "res/meshes/monke.glb" );
	m_skybox = parser.load( "res/meshes/skysphere.glb" );

	wv::Material* skyMaterial = new wv::Material(); // memory leak
	skyMaterial->load( "res/materials/defaultSky.wmat" );
	if( m_skybox ) m_skybox->primitives[ 0 ]->material = skyMaterial;

	wv::Material* phongMaterial = new wv::Material(); // memory leak
	phongMaterial->load( "res/materials/phong.wmat" );
	if( m_player ) m_player->primitives[ 0 ]->material = phongMaterial;

}

void SceneGame::onUnload()
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	device->destroyMesh( &m_player );
	device->destroyMesh( &m_skybox );
}

void SceneGame::onCreate()
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

}

void SceneGame::onDestroy()
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

}

void SceneGame::update( double _deltaTime )
{

}

void SceneGame::draw()
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	/// TODO: remove raw gl calls
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	device->draw( m_skybox );
	glDepthFunc( GL_LESS );
	glDepthMask( GL_TRUE );

	device->draw( m_player );
}
