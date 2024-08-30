#include "SceneGraphWindow.h"

#include <wv/Engine/Engine.h>
#include <wv/Engine/ApplicationState.h>

#include <wv/Scene/SceneRoot.h>
#include <wv/Reflection/ReflectionRegistry.h>
#include <wv/Debug/Print.h>

#include <imgui.h>

void wv::Editor::cSceneGraphWindow::draw( wv::iGraphicsDevice* _pGraphicsDevice )
{
	wv::cEngine* engine = cEngine::get();
	cSceneRoot* sceneRoot = engine->m_pApplicationState->getCurrentScene();

	if( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
		m_selectedObjects.clear();

	
	ImGui::Begin( "SceneGraph" );
	drawSceneObject( sceneRoot, true );
	ImGui::End();

	if( m_openAddChild )
	{
		ImGui::OpenPopup( "AddObject" );
		m_openAddChild = false;
	}



	if( ImGui::BeginPopup( "AddObject" ) )
	{
		auto& classes = wv::cReflectionRegistry::getClasses();
		static const char* selectedClass = nullptr;

		if( ImGui::BeginCombo( "##Class", selectedClass ) )
		{
			for( auto& c : classes )
			{
				bool is_selected = ( selectedClass == c.first.c_str() ); // You can store your selection however you want, outside or inside your objects
			
				if( ImGui::Selectable( c.first.c_str(), is_selected ) )
				{
					selectedClass = c.first.c_str();

					iSceneObject* newObj = ( iSceneObject* )wv::cReflectionRegistry::createInstance( std::string( selectedClass ) );
					if( newObj )
						m_addChildSelected->addChild( newObj, true );
					else
						wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Could not create object of type %s:\n  createInstace is unimplemented\n", selectedClass );
				}

				if( is_selected )
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}
		ImGui::EndCombo();

		ImGui::EndPopup();
	}
}

void wv::Editor::cSceneGraphWindow::drawSceneObject( wv::iSceneObject* _obj, bool _defaultOpen )
{
	if( !_obj )
		return;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
	if( _obj->getChildren().empty() )
		flags |= ImGuiTreeNodeFlags_Leaf;

	if( _defaultOpen )
		flags |= ImGuiTreeNodeFlags_DefaultOpen;

	for( auto& selected : m_selectedObjects )
	{
		if( selected != _obj )
			continue;

		flags |= ImGuiTreeNodeFlags_Selected;
		break;
	}

	bool opened = ImGui::TreeNodeEx( _obj, flags, _obj->getName().c_str() );
	if( ImGui::IsItemClicked( ImGuiMouseButton_Left ) )
	{
		// if ( !ctrl clicking )
			m_selectedObjects.clear();
		
		m_selectedObjects.push_back( _obj );
	}
	if( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
	{
		m_addChildSelected = _obj;
		m_openAddChild     = true;
	}

	if( opened )
	{
		for( auto& child : _obj->getChildren() )
			drawSceneObject( child, false );
		ImGui::TreePop();
	}
	
}