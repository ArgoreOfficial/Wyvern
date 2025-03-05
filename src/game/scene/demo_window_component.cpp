#include "demo_window_component.h"

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/app_state.h>
#include <wv/scene/scene.h>
#include <wv/scene/rigidbody.h>
#include <wv/scene/model.h>
#include <wv/filesystem/file_system.h>

#include <wv/camera/camera.h>
#include <wv/math/ray.h>

#include <wv/scene/component/model_component.h>
#include <wv/scene/component/rigid_body_component.h>

#include <wv/console/console.h>

#include <3ds/3ds_remote_target.h>

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif

void DemoWindowComponent::onEnter( void )
{
	wv::IAppState* state = wv::Engine::get()->m_pAppState;
	wv::Scene* scene = state->getCurrentScene();

	m_mouseMarker = WV_NEW( wv::Entity, wv::Engine::getUniqueUUID(), "mouseMarker" );
	m_mouseMarker->addComponent<wv::ModelComponent>( "meshes/sphere.dae" );
	m_mouseMarker->m_transform.scale = 0.05f;

	scene->addChild( m_mouseMarker );
}

void DemoWindowComponent::onUpdate( double _dt )
{
	wv::IAppState* state = wv::Engine::get()->m_pAppState;
	wv::Vector2i mousePos = wv::Engine::get()->getMousePosition();

	/// TODO: wv::Ray<float>
	wv::Vector3f raystart = state->currentCamera->screenToWorld( mousePos.x, mousePos.y, 0.0f );
	wv::Vector3f rayend   = state->currentCamera->screenToWorld( mousePos.x, mousePos.y, 1.0f );
	wv::Vector3f dir = raystart - rayend;
	dir.normalize();

	m_mouseMarker->m_transform.position = raystart + dir;
}

void DemoWindowComponent::spawnBalls()
{
	wv::Scene* sceneRoot = wv::Engine::get()->m_pAppState->getCurrentScene();

	for( int i = 0; i < m_numToSpawn; i++ )
	{
	#ifdef WV_SUPPORT_PHYSICS
		wv::PhysicsSphereDesc* sphereDesc = WV_NEW( wv::PhysicsSphereDesc );
		sphereDesc->kind = wv::WV_PHYSICS_DYANIMIC;
		sphereDesc->radius = 0.5f;
	#else
		wv::PhysicsSphereDesc* sphereDesc = nullptr;
	#endif

		wv::Rigidbody* rb = WV_NEW( wv::Rigidbody, wv::Engine::getUniqueUUID(), "ball" );
		rb->addComponent<wv::RigidBodyComponent>( sphereDesc );
		rb->addComponent<wv::ModelComponent>( "meshes/ant.dae" );
		rb->m_transform.position.y = 10.0f;
		sceneRoot->addChild( rb );
		m_numSpawned++;
	}
}

void DemoWindowComponent::spawnCubes()
{
	wv::Scene* sceneRoot = wv::Engine::get()->m_pAppState->getCurrentScene();

	for( int i = 0; i < m_numToSpawn; i++ )
	{
	#ifdef WV_SUPPORT_PHYSICS
		wv::PhysicsBoxDesc* boxDesc = WV_NEW( wv::PhysicsBoxDesc );
		boxDesc->kind = wv::WV_PHYSICS_DYANIMIC;
		boxDesc->halfExtent = { 0.5f,0.5f,0.5f };
	#else
		wv::PhysicsBoxDesc* boxDesc = nullptr;
	#endif

		wv::Rigidbody* rb = WV_NEW( wv::Rigidbody, wv::Engine::getUniqueUUID(), "cube" );
		rb->addComponent<wv::RigidBodyComponent>( boxDesc );
		rb->addComponent<wv::ModelComponent>( "meshes/cube.dae" );
		rb->m_transform.position.y = 10.0f;
		sceneRoot->addChild( rb );

		m_numSpawned++;
	}
}

void DemoWindowComponent::spawnBlock( int _halfX, int _halfY, int _halfZ )
{
	wv::Scene* scene = wv::Engine::get()->m_pAppState->getCurrentScene();

	for( int x = -_halfX; x < _halfX; x++ )
	{
		for( int y = -_halfY; y < _halfY; y++ )
		{
			for( int z = -_halfZ; z < _halfZ; z++ )
			{
			#ifdef WV_SUPPORT_PHYSICS
				wv::PhysicsBoxDesc* boxDesc = WV_NEW( wv::PhysicsBoxDesc );
				boxDesc->kind = wv::WV_PHYSICS_DYANIMIC;
				boxDesc->halfExtent = { 0.5f,0.5f,0.5f };
			#else
				wv::PhysicsBoxDesc* boxDesc = nullptr;
			#endif

				wv::Rigidbody* rb = WV_NEW( wv::Rigidbody, wv::Engine::getUniqueUUID(), "cube" );
				rb->m_transform.position = { (float)x, (float)y + _halfY - 6.0f, (float)z };
				rb->addComponent<wv::RigidBodyComponent>( boxDesc );
				rb->addComponent<wv::ModelComponent>( "meshes/cube.dae" );
				scene->addChild( rb );

				m_numSpawned++;
			}
		}
	}
}

void DemoWindowComponent::drawDemoWindow()
{
#ifdef WV_SUPPORT_IMGUI
	ImGui::Begin( "Wyvern Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize );

	ImGui::InputInt( "Spawn Count", &m_numToSpawn );

	if( ImGui::Button( "Spawn Balls" ) )
		spawnBalls();

	ImGui::SameLine();
	if( ImGui::Button( "Spawn Boxes" ) )
		spawnCubes();

	ImGui::SameLine();
	if( ImGui::Button( "Spawn Block" ) )
		spawnBlock( 5, m_numToSpawn / 2, 5 );

	ImGui::Text( "RigidBodies Spawned: %i", m_numSpawned );
	ImGui::Text( "Bytes Allocated: %i", wv::MemoryTracker::getTotalAllocationSize() );
	
	ImGui::End();
#endif
}

void DemoWindowComponent::onDraw( wv::IDeviceContext* /*_context*/, wv::IGraphicsDevice* /*_device*/ )
{
#ifdef WV_SUPPORT_IMGUI
	drawDemoWindow();
#endif
}

