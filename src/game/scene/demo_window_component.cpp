#include "demo_window_component.h"

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/app_state.h>
#include <wv/scene/scene.h>
#include <wv/scene/rigidbody.h>
#include <wv/scene/model.h>
#include <wv/filesystem/file_system.h>

#include <wv/scene/component/model_component.h>
#include <wv/scene/component/rigid_body_component.h>

#include <wv/console/command.h>

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif

void DemoWindowComponent::spawnBalls()
{
	wv::Scene* sceneRoot = wv::Engine::get()->m_pApplicationState->getCurrentScene();

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
	wv::Scene* sceneRoot = wv::Engine::get()->m_pApplicationState->getCurrentScene();

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
	wv::Scene* scene = wv::Engine::get()->m_pApplicationState->getCurrentScene();

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

void DemoWindowComponent::drawBuildWindow()
{
#ifdef WV_SUPPORT_IMGUI


	ImGui::BeginDisabled( mIsBuilding3DS );

	if( ImGui::Button( "Build" ) )
		buildPlatform();

	if( ImGui::Button( "Build & Run" ) )
		buildAndRun();

	ImGui::EndDisabled();

	ImGui::SetNextItemWidth( 100.0f );
	ImGui::InputText( "##ipinput", mTargetAddressStr, 16 );
	ImGui::SetNextItemWidth( 38.0f );
	ImGui::InputInt( "##portinput", &mTargetPort, 0 );

	ImGui::SetNextItemWidth( 100.0f );
	if( ImGui::BeginCombo( "##plat", mCurrentBuildPlatform ) )
	{
		for( int i = 0; i < mBuildPlatforms.size(); i++ )
		{
			bool is_selected = ( mCurrentBuildPlatform == mBuildPlatforms[ i ] );
			
			if( ImGui::Selectable( mBuildPlatforms[ i ], is_selected ) )
				mCurrentBuildPlatform = mBuildPlatforms[ i ];
			
			if( is_selected )
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SetNextItemWidth( 100.0f );
	if( ImGui::BeginCombo( "##mode", mCurrentBuildMode ) )
	{
		for( int i = 0; i < mBuildModes.size(); i++ )
		{
			bool is_selected = ( mCurrentBuildMode == mBuildModes[ i ] );

			if( ImGui::Selectable( mBuildModes[ i ], is_selected ) )
				mCurrentBuildMode = mBuildModes[ i ];

			if( is_selected )
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	
#endif
}

void DemoWindowComponent::buildPlatform()
{
	if( mIsBuilding3DS )
		return;

	mIsBuilding3DS = true;

	wv::JobSystem* pJobSystem = wv::Engine::get()->m_pJobSystem;

	if( mBuildFence == nullptr )
		mBuildFence = pJobSystem->createFence();

	wv::Job::JobFunction_t fptr = [&](void*)
		{

			for( int i = 0; i < mBuildPlatforms.size(); i++ )
				if( mCurrentBuildPlatform == mBuildPlatforms[ i ] )
					mCurrentBuildArch = mBuildArchs[ i ];
			
			wv::Debug::Print( "Launching xmake build\n" );
			wv::Debug::Print( "    %s(%s) : %s\n", mCurrentBuildPlatform, mCurrentBuildArch, mCurrentBuildMode );
			
			{
				wv::Console::run( "../../", {
					"xmake",
					"f -c",
					"-a", mCurrentBuildArch,
					"-p", mCurrentBuildPlatform,
					"-m", mCurrentBuildMode 
				} );
			}

			{
				if( wv::Console::run( { "xmake" } ) )
					wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Compilation Failed\n" );
				else
					wv::Debug::Print( "Done!\n" );
			}

			mIsBuilding3DS = false;
		};

	wv::Job* job = pJobSystem->createJob( fptr, mBuildFence );
	pJobSystem->submit( { job } );
}

void DemoWindowComponent::buildAndRun()
{
	buildPlatform();
	
	wv::JobSystem* pJobSystem = wv::Engine::get()->m_pJobSystem;
	wv::Job::JobFunction_t fptr = [ & ]( void* )
		{
			wv::Engine::get()->m_pJobSystem->waitForFence( mBuildFence );

			wv::Debug::Print( "Launching\n" );

			std::string launchFile = "Sandbox_" + std::string( mCurrentBuildMode ) + "_arm_3ds";

			int err = wv::Console::run( {
					"../../tools/3ds/3dslink",
					"-a", mTargetAddressStr,
					"../3ds/" + launchFile + ".3dsx"
				} );

			if ( err )
			{
				wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Run Failed\n" );
				return;
			}
			else
				wv::Debug::Print( "Launched on %s:%i\n", mTargetAddressStr, mTargetPort );
			
			/// TODO
			// cmd : arm-none-eabi-gdb Sandbox_Release_arm_3ds.elf
			// cmd : target remote 192.168.0.160:4003

		};
	wv::Job* job = pJobSystem->createJob( fptr );
	pJobSystem->submit( { job } );
}

void DemoWindowComponent::onDraw( wv::IDeviceContext* /*_context*/, wv::IGraphicsDevice* /*_device*/ )
{
#ifdef WV_SUPPORT_IMGUI
	drawDemoWindow();
	
	if( ImGui::BeginMainMenuBar() )
		drawBuildWindow();
	ImGui::EndMainMenuBar();
#endif
}

