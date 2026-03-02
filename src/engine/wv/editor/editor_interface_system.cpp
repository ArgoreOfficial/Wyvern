#include "editor_interface_system.h"

#include <wv/application.h>
#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/reflection/reflection.h>
#include <wv/rendering/renderer.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

void componentTreeNode( wv::IEntityComponent* _comp, wv::SceneViewSelection* _selection )
{
	ImGuiTreeNodeFlags compFlag = ImGuiTreeNodeFlags_DefaultOpen;
	compFlag |= ImGuiTreeNodeFlags_Leaf;

	if ( _comp == _selection->selectedComponent )
		compFlag |= ImGuiTreeNodeFlags_Selected;

	std::string compLabel = std::format( "{} ({})", _comp->getTypeName(), _comp->getID().toString() );

	if ( ImGui::TreeNodeEx( compLabel.c_str(), compFlag ) )
	{
		if ( ImGui::IsItemClicked() )
		{
			*_selection = {};

			_selection->selectedComponent = _comp;
		}

		ImGui::TreePop();
	}
}

void entityTreeNode( wv::Entity* _entity, wv::SceneViewSelection* _selection )
{
	auto components = _entity->getComponents();

	ImGuiTreeNodeFlags flags = 0; // ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_OpenOnArrow;

	if ( components.empty() )
		flags |= ImGuiTreeNodeFlags_Leaf;

	if ( _entity == _selection->selectedEntity )
		flags |= ImGuiTreeNodeFlags_Selected;
	
	std::string entLabel = std::format( "{} ({})", _entity->getName(), _entity->getID().toString() );

	bool isOpen = ImGui::TreeNodeEx( entLabel.c_str(), flags );
	
	if ( ImGui::IsItemClicked() )
	{
		*_selection = {};
		_selection->selectedEntity = _entity;
	}

	if ( isOpen )
	{
		for ( auto comp : components )
			componentTreeNode( comp, _selection );
		
		ImGui::TreePop();
	}
}

void worldSystemTreeNode( wv::IWorldSystem* _sys, wv::SceneViewSelection* _selection )
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_Leaf;

	if ( _sys == _selection->selectedWorldSystem )
		flags |= ImGuiTreeNodeFlags_Selected;

	std::string label = std::format( "{}", _sys->getTypeName() );

	bool isOpen = ImGui::TreeNodeEx( label.c_str(), flags );

	if ( ImGui::IsItemClicked() )
	{
		*_selection = {};
		_selection->selectedWorldSystem = _sys;
	}

	if( isOpen )
	{
		ImGui::TreePop();
	}
}

void wv::EditorInterfaceSystem::update( WorldUpdateContext& _ctx )
{
	m_timeSinceFPSUpdate += _ctx.deltaTime;
	m_framesSinceFPSUpdate++;

	if ( m_timeSinceFPSUpdate > 0.5 )
	{
		double averageDt = m_timeSinceFPSUpdate / m_framesSinceFPSUpdate;

		if( averageDt != 0 )
			m_averageFPS = 1.0 / averageDt;

		m_timeSinceFPSUpdate -= 0.5;
		m_framesSinceFPSUpdate = 0;
	}
}

void wv::EditorInterfaceSystem::onDebugRender()
{
#ifdef WV_SUPPORT_IMGUI
	renderPrimaryMenuBar();
	renderSecondaryMenuBar();
	renderStatusBar();

	if ( m_showRenderWorldWindow )
		renderWorldWindow();

#endif
}

void wv::EditorInterfaceSystem::renderPrimaryMenuBar()
{
	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( "Tools" ) )
		{
			if ( ImGui::MenuItem( "World" ) )
				m_showRenderWorldWindow = !m_showRenderWorldWindow;
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}

void wv::EditorInterfaceSystem::renderSecondaryMenuBar()
{
	ImGuiViewportP* viewport = (ImGuiViewportP*)ImGui::GetMainViewport();

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();

	if ( ImGui::BeginViewportSideBar( "##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags ) )
	{
		if ( ImGui::BeginMenuBar() )
		{
			//drawBuildWindow();
			//m_runComboButton.draw();
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}

void wv::EditorInterfaceSystem::renderStatusBar()
{
	ImGuiViewportP* viewport = (ImGuiViewportP*)ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();

	double deltaTime = wv::Application::getSingleton()->getDeltaTime();

	if ( ImGui::BeginViewportSideBar( "##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags ) )
	{
		if ( ImGui::BeginMenuBar() )
		{
			ImGui::Text( "FPS: %f", m_averageFPS );
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}

void wv::EditorInterfaceSystem::renderWorldWindow()
{
#ifdef WV_SUPPORT_IMGUI
	auto entities = m_components.getEntities();
	Entity* selectedEntity = nullptr;
	IEntityComponent* selectedComponent = nullptr;

	if ( ImGui::Begin( "World" ) )
	{
		ImGui::SeparatorText( "World Systems" );

		Application* app = Application::getSingleton();
		World* world = app->getWorld();
		auto worldSystems = world->getWorldSystems();

		ImVec2 wsize = ImGui::GetWindowSize();

		if ( ImGui::BeginChild( "###WorldSystemTree", ImVec2( -FLT_MIN, wsize.y / 4 ) ) )
		{
			for ( IWorldSystem* worldSystem : worldSystems )
				worldSystemTreeNode( worldSystem, &m_selection );
		}
		ImGui::EndChild();

		ImGui::SeparatorText( "Entities" );

		if ( ImGui::BeginChild( "###SceneViewTree", ImVec2( -FLT_MIN, wsize.y / 4 ) ) )
		{
			for ( size_t i = 0; i < entities.size(); i++ )
			{
				Entity* ent = entities[ i ];
				entityTreeNode( ent, &m_selection );
			}
		}
		ImGui::EndChild();

		if ( ImGui::BeginChild( "###SelectedProperties", ImVec2( -FLT_MIN, wsize.y / 4 ) ) )
		{
			wv::IReflectedType* reflt = nullptr;

			if ( m_selection.selectedEntity )
			{
				Entity* ent = m_selection.selectedEntity;
				ImGui::SeparatorText( "Entity" );

				bool debugDisplayEnabled = ent->getDebugDisplayEnabled();
				ImGui::Checkbox( "Show Debug", &debugDisplayEnabled );
				ent->setDebugDisplayEnabled( debugDisplayEnabled );

				reflt = (wv::IReflectedType*)ent;

				wv::Vector3f arrowPos{ ent->getTransform().position };
				float time = app->getApplicationTime() * 5.0f;
				arrowPos += { 0.0f, 0.5f * std::sinf( time ) + 0.5f, 0.0f };

				app->getRenderer()->addDebugLine( arrowPos, arrowPos + Vector3f{ 0.0f, 3.0f, 0.0 } );
				app->getRenderer()->addDebugLine( arrowPos, arrowPos + Vector3f{ 0.5f, 0.5f, 0.0 } );
				app->getRenderer()->addDebugLine( arrowPos, arrowPos + Vector3f{ -0.5f, 0.5f, 0.0 } );
			}
			else if ( m_selection.selectedComponent )
			{
				IEntityComponent* comp = m_selection.selectedComponent;
				ImGui::SeparatorText( comp->getTypeName().c_str() );
				reflt = (wv::IReflectedType*)comp;
			}
			else if ( m_selection.selectedWorldSystem )
			{
				IWorldSystem* sys = m_selection.selectedWorldSystem;
				ImGui::SeparatorText( sys->getTypeName().c_str() );
				reflt = (wv::IReflectedType*)sys;
			}

			// Display reflected data

			if ( reflt )
			{
				auto inherited = reflt->getInheritedUUIDs();

				// Iterate backwards, so that inherited members appear before 

				for ( auto it = inherited.rbegin(); it != inherited.rend(); it++ )
				{
					wv::TypeInfo* typeInfo = wv::reflreg()->getTypeInfo( *it );
					if ( !typeInfo )
						continue;

					for ( auto* member : typeInfo->members )
					{
						std::string memberValue = std::format( "{}: {}", member->displayName, member->format( reflt ) );
						ImGui::Text( "%s", memberValue.c_str() );
					}
				}

				// Iterate final members

				{
					wv::TypeUUID typeUUID = reflt->getTypeUUID();
					wv::TypeInfo* typeInfo = wv::reflreg()->getTypeInfo( typeUUID );

					for ( auto* member : typeInfo->members )
					{
						std::string memberValue = std::format( "{}: {}", member->displayName, member->format( reflt ) );
						ImGui::Text( "%s", memberValue.c_str() );
					}
				}
			}
			else
			{
				// if nothing is selected, just do a basic separator
				ImGui::SeparatorText( "" );
			}
		}
		ImGui::EndChild();

	}
	ImGui::End();
#endif
}
