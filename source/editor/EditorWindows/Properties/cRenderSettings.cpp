#include "cRenderSettings.h"

#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>
#include <wv/Camera/iCamera.h>

cRenderSettings::cRenderSettings( void )
{

}

cRenderSettings::~cRenderSettings( void )
{

}

void cRenderSettings::render( void )
{
	wv::cRenderer& renderer = wv::cRenderer::getInstance();
	wv::cApplication& application = wv::cApplication::getInstance();

	if ( ImGui::Begin( "Render Settings" ) )
	{
		// MSAA toggle
		if ( ImGui::Checkbox( "MSAA", &m_msaa ) )
			renderer.createGBuffer( 4 * (int)m_msaa );
		
		// rendermode
		const char* render_modes[] = { "None", "Albedo", "Normal", "Position", "Roughness", "Metallic", "Depth", "Light" };
		static const char* selected_render_mode = render_modes[ 0 ];

		if ( ImGui::BeginCombo( "Debug RenderMode", selected_render_mode ) )
		{
			for ( int n = 0; n < IM_ARRAYSIZE( render_modes ); n++ )
			{
				bool is_selected = ( selected_render_mode == render_modes[ n ] );
				if ( ImGui::Selectable( render_modes[ n ], is_selected ) )
				{
					selected_render_mode = render_modes[ n ];
					renderer.setDebugRenderMode( n );

					if ( is_selected )
						ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// exposure and gamma
		ImGui::InputFloat( "Gamma", &renderer.gamma );
		ImGui::InputFloat( "Exposure", &renderer.exposure );
		ImGui::InputFloat( "FOV", &application.m_camera3D->fov );
	}
	ImGui::End();

}
