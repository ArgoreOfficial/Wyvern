#include "cDeferredLightRenderPass.h"

#include <cm/Backends/iBackend.h>
#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>
#include <wv/Scene/cSceneManager.h>
#include <wv/Managers/cContentManager.h>
#include <wv/Camera/iCamera.h>

#include <wv/Graphics/cShader.h>
#include <wv/Graphics/cMesh.h>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>

void wv::cDeferredLightRenderPass::onCreate( void )
{
	m_shader = cContentManager::getInstance().getShader( "res/shaders/deferred/s_screen" );

	m_framebuffer = new cFramebuffer();
	m_framebuffer->create();
	m_framebuffer->addTexture( "gPosition", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_framebuffer->addTexture( "gNormal", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_framebuffer->addTexture( "gAlbedo", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_framebuffer->addTexture( "gLight", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_framebuffer->finalize();
}

void wv::cDeferredLightRenderPass::onDestroy( void )
{
	delete m_framebuffer;
}

void wv::cDeferredLightRenderPass::execute( cFramebuffer* _input_buffer )
{
	// TODO: separate out into lightpass and assembler pass

	cRenderer&     renderer       = cRenderer::getInstance();
	cSceneManager& scene_manager  = cSceneManager::getInstance();
	cApplication&  application    = cApplication::getInstance();
	cm::iBackend*  backend        = renderer.getBackend();

	cVector3f cam_dir = application.m_current_camera->getViewDirection();
	cVector3f cam_pos = application.m_current_camera->getTransform().position;

	cVector3f dirl = scene_manager.getDirectionalLightDirection();
	dirl.normalize();
	float dirl_intensity = scene_manager.getDirectionalLightIntensity();
	float ambl_intensity = scene_manager.getAmbientLightIntensity();
	int   numlights      = scene_manager.light_positions.size();

	backend->useShaderProgram( m_shader->shader_program_handle );
	_input_buffer->bindTexturesToShader( m_shader );

	renderer.clear( 0x44A5FF00, cm::eClearMode::ClearMode_Color | cm::ClearMode_Depth );

	m_shader->ubBegin();
	m_shader->ubBufferData( "uRenderMode",                &renderer.debug_render_mode,          sizeof( int ) );
	m_shader->ubBufferData( "uDirectionalLight",          &dirl,                                sizeof( cVector3f ) );
	m_shader->ubBufferData( "uAmbientLightIntensity",     &ambl_intensity,                      sizeof( float ) );
	m_shader->ubBufferData( "uDirectionalLightIntensity", &dirl_intensity,                      sizeof( float ) );
	m_shader->ubBufferData( "uCameraDirection",           &cam_dir,                             sizeof( cVector3f ) );
	m_shader->ubBufferData( "uCameraPosition",            &cam_pos,                             sizeof( cVector3f ) );
	m_shader->ubBufferData( "uNumPointLights",            &numlights,                           sizeof( int ) );
	m_shader->ubBufferData( "uLightPos[0]",               scene_manager.light_positions.data(), sizeof( cVector4f ) * numlights );
	m_shader->ubBufferData( "uLightCol[0]",               scene_manager.light_colors.data(),    sizeof( cVector4f ) * numlights );
	m_shader->ubEnd();
}
