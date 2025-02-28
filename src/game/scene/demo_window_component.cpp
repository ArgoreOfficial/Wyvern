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

#include <wv/console/console.h>

#include <3ds/3ds_remote_target.h>

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif

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

void DemoWindowComponent::drawBuildWindow()
{
#ifdef WV_SUPPORT_IMGUI
	ImGui::SetNextItemWidth( ImGui::CalcTextSize( "255.255.255.255 " ).x );
	ImGui::InputText( "##ipinput", m_targetAddressStr, 16, ImGuiInputTextFlags_CharsDecimal );

	ImGui::SetNextItemWidth( ImGui::CalcTextSize( "65535 " ).x );
	ImGui::InputInt( "##portinput", &m_targetPort, 0 );

	ImGui::SetNextItemWidth( 100.0f );
	if( ImGui::BeginCombo( "##plat", m_currentBuildPlatform ) )
	{
		for( int i = 0; i < m_buildPlatforms.size(); i++ )
		{
			bool is_selected = ( m_currentBuildPlatform == m_buildPlatforms[ i ] );
			
			if( ImGui::Selectable( m_buildPlatforms[ i ], is_selected ) )
				m_currentBuildPlatform = m_buildPlatforms[ i ];
			
			if( is_selected )
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SetNextItemWidth( 100.0f );
	if( ImGui::BeginCombo( "##mode", m_currentBuildMode ) )
	{
		for( int i = 0; i < m_buildModes.size(); i++ )
		{
			bool is_selected = ( m_currentBuildMode == m_buildModes[ i ] );

			if( ImGui::Selectable( m_buildModes[ i ], is_selected ) )
				m_currentBuildMode = m_buildModes[ i ];

			if( is_selected )
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	
#endif
}

void DemoWindowComponent::buildPlatform()
{
	if( m_isBuilding3DS )
		return;

	m_isBuilding3DS = true;

	wv::JobSystem* pJobSystem = wv::Engine::get()->m_pJobSystem;

	if( m_buildFence == nullptr )
		m_buildFence = pJobSystem->createFence();

	wv::Job::JobFunction_t fptr = [&](void*)
		{

			for( int i = 0; i < m_buildPlatforms.size(); i++ )
				if( m_currentBuildPlatform == m_buildPlatforms[ i ] )
					m_currentBuildArch = m_buildArchs[ i ];
			
			wv::Debug::Print( "Launching xmake build\n" );
			wv::Debug::Print( "    %s(%s) : %s\n", m_currentBuildPlatform, m_currentBuildArch, m_currentBuildMode );
			
			{
				wv::Console::run( "../../", {
					"xmake",
					"f -c",
					"-a", m_currentBuildArch,
					"-p", m_currentBuildPlatform,
					"-m", m_currentBuildMode 
				} );
			}

			{
				if( wv::Console::run( { "xmake" } ) )
					wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Compilation Failed\n" );
				else
					wv::Debug::Print( "Done!\n" );
			}

			m_isBuilding3DS = false;
		};

	wv::Job* job = pJobSystem->createJob( fptr, m_buildFence );
	pJobSystem->submit( { job } );
}

void DemoWindowComponent::buildAndRun()
{
	buildPlatform();
	
	wv::JobSystem* pJobSystem = wv::Engine::get()->m_pJobSystem;
	wv::Job::JobFunction_t fptr = [ & ]( void* )
		{
			wv::Engine::get()->m_pJobSystem->waitForFence( m_buildFence );
			std::string launchFile = "Sandbox_" + std::string( m_currentBuildMode ) + "_arm_3ds";

			wv::Debug::Print( "Launching\n" );
			wv::RemoteTarget3DS target( m_targetAddressStr, m_targetPort );

			if( target.remoteLaunchExecutable( launchFile, {} ) )
			{
				wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Run Failed\n" );
				return;
			}
			else
				wv::Debug::Print( "Launched on %s:%i\n", m_targetAddressStr, m_targetPort );

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
	{
		if( ImGui::BeginMenu( "File" ) )
		{
			if( ImGui::MenuItem( "Create" ) )         { WV_WARNING( "Unimplemented\n" ); }
			if( ImGui::MenuItem( "Open", "Ctrl+O" ) ) { WV_WARNING( "Unimplemented\n" ); }
			if( ImGui::MenuItem( "Save", "Ctrl+S" ) ) { WV_WARNING( "Unimplemented\n" ); }
			if( ImGui::MenuItem( "Save as.." ) )      { WV_WARNING( "Unimplemented\n" ); }
			ImGui::EndMenu();
		}

		if( ImGui::BeginMenu( "Build" ) )
		{
			ImGui::BeginDisabled( m_isBuilding3DS );
			if( ImGui::MenuItem( "Build" ) ) 
				buildPlatform();

			if( ImGui::MenuItem( "Build & Run" ) ) 
				buildAndRun();
			ImGui::EndDisabled();

			if( ImGui::MenuItem( "Target Manager" ) ) { WV_WARNING( "Unimplemented\n" ); }
			ImGui::EndMenu();
		}
		
		drawBuildWindow();
		
		ImGui::EndMainMenuBar();
	}
#endif
}

