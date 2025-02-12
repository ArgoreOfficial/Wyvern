#include "DemoWindowComponent.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/GraphicsDevice.h>

#include <wv/Engine/ApplicationState.h>
#include <wv/Scene/Scene.h>
#include <wv/Scene/Rigidbody.h>
#include <wv/Scene/Model.h>
#include <wv/Memory/FileSystem.h>

#include <wv/Scene/Component/ModelComponent.h>
#include <wv/Scene/Component/RigidBodyComponent.h>

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif

void DemoWindowComponent::spawnBalls( int _count )
{
	wv::Scene* sceneRoot = wv::cEngine::get()->m_pApplicationState->getCurrentScene();

	for( int i = 0; i < m_numToSpawn; i++ )
	{
		wv::sPhysicsSphereDesc* sphereDesc = WV_NEW( wv::sPhysicsSphereDesc );
		sphereDesc->kind = wv::WV_PHYSICS_DYANIMIC;
		sphereDesc->radius = 0.5f;

		wv::cRigidbody* rb = WV_NEW( wv::cRigidbody, wv::cEngine::getUniqueUUID(), "ball" );
		rb->addComponent<wv::RigidBodyComponent>( sphereDesc );
		rb->addComponent<wv::ModelComponent>( "meshes/sphere.dae" );
		sceneRoot->addChild( rb );
		m_numSpawned++;
	}
}

void DemoWindowComponent::spawnCubes( int _count )
{
	wv::Scene* sceneRoot = wv::cEngine::get()->m_pApplicationState->getCurrentScene();

	for( int i = 0; i < m_numToSpawn; i++ )
	{
		wv::sPhysicsBoxDesc* boxDesc = WV_NEW( wv::sPhysicsBoxDesc );
		boxDesc->kind = wv::WV_PHYSICS_DYANIMIC;
		boxDesc->halfExtent = { 0.5f,0.5f,0.5f };

		wv::cRigidbody* rb = WV_NEW( wv::cRigidbody, wv::cEngine::getUniqueUUID(), "cube" );
		rb->addComponent<wv::RigidBodyComponent>( boxDesc );
		rb->addComponent<wv::ModelComponent>( "meshes/cube.dae" );
		rb->m_transform.position.y = 1.0f;
		sceneRoot->addChild( rb );

		m_numSpawned++;
	}
}

void DemoWindowComponent::spawnBlock( int _halfX, int _halfY, int _halfZ )
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

				wv::cRigidbody* rb = WV_NEW( wv::cRigidbody, wv::cEngine::getUniqueUUID(), "cube" );
				rb->m_transform.position = { (float)x, (float)y + _halfY - 6.0f, (float)z };
				rb->addComponent<wv::RigidBodyComponent>( boxDesc );
				rb->addComponent<wv::ModelComponent>( "meshes/cube.dae" );
				scene->addChild( rb );

				m_numSpawned++;
			}
		}
	}
}

void DemoWindowComponent::onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device )
{
#ifdef WV_SUPPORT_IMGUI
	ImGui::Begin( "Wyvern Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize );

	ImGui::InputInt( "Spawn Count", &m_numToSpawn );

	if( ImGui::Button( "Spawn Balls" ) )
		spawnBalls( m_numToSpawn );

	ImGui::SameLine();
	if( ImGui::Button( "Spawn Boxes" ) )
		spawnCubes( m_numToSpawn );

	ImGui::SameLine();
	if( ImGui::Button( "Spawn Block" ) )
		spawnBlock( 5, m_numToSpawn / 2, 5 );

	ImGui::Text( "RigidBodies Spawned: %i", m_numSpawned );
	ImGui::SameLine();

	ImGui::End();
#endif
}
