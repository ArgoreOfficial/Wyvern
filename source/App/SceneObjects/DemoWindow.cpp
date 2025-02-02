#include "DemoWindow.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/Graphics.h>

#include <wv/Engine/ApplicationState.h>
#include <wv/Scene/Scene.h>
#include <wv/Scene/Rigidbody.h>
#include <wv/Scene/Model.h>
#include <wv/Memory/FileSystem.h>

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
	wv::Scene* sceneRoot = wv::cEngine::get()->m_pApplicationState->getCurrentScene();

	for ( int i = 0; i < m_numToSpawn; i++ )
	{
		wv::sPhysicsSphereDesc* sphereDesc = WV_NEW( wv::sPhysicsSphereDesc );
		sphereDesc->kind = wv::WV_PHYSICS_DYANIMIC;
		sphereDesc->radius = 0.5f;

		wv::cRigidbody* rb = WV_NEW( wv::cRigidbody, wv::cEngine::getUniqueUUID(), "ball", "res/meshes/ant.dae", sphereDesc );
		sceneRoot->addChild( rb );
		m_numSpawned++;
	}
	sceneRoot->onCreate();
	sceneRoot->onLoad();
}

///////////////////////////////////////////////////////////////////////////////////////

void cDemoWindow::spawnCubes( int _count )
{
	wv::Scene* sceneRoot = wv::cEngine::get()->m_pApplicationState->getCurrentScene();

	for ( int i = 0; i < m_numToSpawn; i++ )
	{
		wv::sPhysicsBoxDesc* boxDesc = WV_NEW( wv::sPhysicsBoxDesc );
		boxDesc->kind = wv::WV_PHYSICS_DYANIMIC;
		boxDesc->halfExtent = { 0.5f,0.5f,0.5f };
		
		wv::cRigidbody* rb = WV_NEW( wv::cRigidbody, wv::cEngine::getUniqueUUID(), "cube", "res/meshes/termite.dae", boxDesc );
		rb->m_transform.position.y = 1.0f;
		sceneRoot->addChild( rb );
		
		m_numSpawned++;
	}
	sceneRoot->onCreate();
	sceneRoot->onLoad();
}

///////////////////////////////////////////////////////////////////////////////////////

void cDemoWindow::spawnBlock( int _halfX, int _halfY, int _halfZ )
{
	wv::Scene* scene = wv::cEngine::get()->m_pApplicationState->getCurrentScene();

	for( int x = -_halfX; x < _halfX; x++ )
	{
		for( int y = -_halfY; y < _halfY; y++ )
		{
			for( int z = -_halfZ; z < _halfZ; z++ )
			{
				wv::sPhysicsBoxDesc* boxDesc = WV_NEW( wv::sPhysicsBoxDesc );
				boxDesc->kind = wv::WV_PHYSICS_DYANIMIC;
				boxDesc->halfExtent = { 0.5f,0.5f,0.5f };
				
				wv::cRigidbody* rb = WV_NEW( wv::cRigidbody, wv::cEngine::getUniqueUUID(), "cube", "res/meshes/cube.dae", boxDesc );
				rb->m_transform.position = { (float)x, (float)y + _halfY - 6.0f, (float)z };
				scene->addChild( rb );

				m_numSpawned++;
			}
		}
	}

	scene->onCreate();
	scene->onLoad();
}

///////////////////////////////////////////////////////////////////////////////////////

void cDemoWindow::onUpdate( double _deltaTime )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void cDemoWindow::drawImpl( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device )
{
#ifdef WV_SUPPORT_IMGUI
	ImGui::Begin( "Wyvern Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize );
	
	ImGui::InputInt( "Spawn Count", &m_numToSpawn );
	
	if( ImGui::Button( "Spawn Balls" ) )
		spawnBalls( m_numToSpawn );
	
	ImGui::SameLine();
	if ( ImGui::Button( "Spawn Boxes" ) )
		spawnCubes( m_numToSpawn );
	
	ImGui::SameLine();
	if ( ImGui::Button( "Spawn Block" ) )
		spawnBlock( 5, 5, 5 );

	ImGui::Text( "RigidBodies Spawned: %i", m_numSpawned );
	ImGui::SameLine();

	ImGui::End();
#endif
}