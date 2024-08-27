#include "DemoWindow.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Engine/ApplicationState.h>
#include <wv/Scene/SceneRoot.h>
#include <wv/Scene/Rigidbody.h>

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////

cDemoWindow::cDemoWindow( const uint64_t& _uuid, const std::string& _name ) :
	iSceneObject{ _uuid, _name }
{

}

///////////////////////////////////////////////////////////////////////////////////////

cDemoWindow::~cDemoWindow()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void cDemoWindow::spawnBalls( int _count )
{
	wv::cSceneRoot* sceneRoot = wv::cEngine::get()->m_pApplicationState->getCurrentScene();

	for ( int i = 0; i < m_numToSpawn; i++ )
	{
		wv::sPhysicsSphereDesc* sphereDesc = new wv::sPhysicsSphereDesc();
		sphereDesc->kind = wv::WV_PHYSICS_DYANIMIC;
		sphereDesc->radius = 0.5f;

		wv::cRigidbody* rb = new wv::cRigidbody( wv::cEngine::getUniqueUUID(), "ball", "res/meshes/debug-sphere.dae", sphereDesc );
		sceneRoot->addChild( rb );
		m_numSpawned++;
	}
	sceneRoot->onCreate();
	sceneRoot->onLoad();
}

void cDemoWindow::spawnCubes( int _count )
{
	wv::cSceneRoot* sceneRoot = wv::cEngine::get()->m_pApplicationState->getCurrentScene();

	for ( int i = 0; i < m_numToSpawn; i++ )
	{
		wv::sPhysicsBoxDesc* boxDesc = new wv::sPhysicsBoxDesc();
		boxDesc->kind = wv::WV_PHYSICS_DYANIMIC;
		boxDesc->halfExtent = { 0.5f,0.5f,0.5f };

		wv::cRigidbody* rb = new wv::cRigidbody( wv::cEngine::getUniqueUUID(), "cube", "", boxDesc );
		sceneRoot->addChild( rb );
		m_numSpawned++;
	}
	sceneRoot->onCreate();
	sceneRoot->onLoad();
}

void cDemoWindow::updateImpl( double _deltaTime )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void cDemoWindow::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
#ifdef WV_SUPPORT_IMGUI
	ImGui::Begin( "Wyvern Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize );
	
	ImGui::InputInt( "Spawn Count", &m_numToSpawn );
	
	if( ImGui::Button( "Spawn Balls" ) )
		spawnBalls( m_numToSpawn );
	
	ImGui::SameLine();
	if ( ImGui::Button( "Spawn Boxes" ) )
		spawnCubes( m_numToSpawn );
	
	ImGui::Text( "RigidBodies Spawned: %i", m_numSpawned );
	ImGui::SameLine();

	ImGui::End();
#endif
}