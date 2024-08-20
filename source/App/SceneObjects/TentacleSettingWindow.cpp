#include "TentacleSettingWindow.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <imgui.h>

///////////////////////////////////////////////////////////////////////////////////////

cTentacleSettingWindowObject::cTentacleSettingWindowObject( const uint64_t& _uuid, const std::string& _name ) :
	iSceneObject{ _uuid, _name }
{

}

///////////////////////////////////////////////////////////////////////////////////////

cTentacleSettingWindowObject::~cTentacleSettingWindowObject()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void cTentacleSettingWindowObject::updateImpl( double _deltaTime )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void cTentacleSettingWindowObject::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
	ImGui::Begin( "Tentacle Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize );
	
	for( int i = 0; i < 3; i++ )
	{
		
		settings[ i ].drawInputs( i );

		if( i != 2 )
		{
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
		}
	}

	ImGui::End();
}

void sTentacleSetting::drawInputs( int _i )
{
	std::string n = std::to_string( _i );

	std::string f = "##frequ" + n;
	std::string p = "##phase" + n;
	std::string a = "##ampli" + n;
	std::string s = "##shift" + n;

	ImGui::BeginGroup();
	{
		ImGui::DragFloat( f.c_str(), &frequency, 0.1f );
		ImGui::SameLine();
		ImGui::Text( "Frequency " );

		ImGui::DragFloat( p.c_str(), &phase, 0.1f );
		ImGui::SameLine();
		ImGui::Text( "Phase " );

		ImGui::DragFloat( a.c_str(), &amplitude, 0.1f );
		ImGui::SameLine();
		ImGui::Text( "Amplitude " );

		ImGui::DragFloat( s.c_str(), &shift, 0.1f );
		ImGui::SameLine();
		ImGui::Text( "Shift " );
	}
	ImGui::EndGroup();

	ImVec2 v = ImVec2( ImGui::GetItemRectMax().x * 1.5, ImGui::GetItemRectMax().y * 1.5 );
	ImGui::GetForegroundDrawList()->AddRect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32( 255, 255, 255, 128 ) );

}

float sTentacleSetting::getValue( float _t )
{
	return sinf( ( float )_t * frequency + phase ) * amplitude + shift;
}
