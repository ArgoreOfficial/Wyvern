#include "cDeferredLightRenderPass.h"

#include <cm/Backends/iBackend.h>

#include <wv/Camera/iCamera.h>
#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>
#include <wv/Graphics/cShader.h>
#include <wv/Graphics/cMesh.h>
#include <wv/Managers/cContentManager.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector4.h>
#include <wv/Scene/cSceneManager.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void wv::cDeferredLightRenderPass::onCreate( void )
{
	m_shader = cContentManager::getInstance().getShader( "res/shaders/deferred/s_light_pass" );

	m_framebuffer = new cFramebuffer();
	m_framebuffer->create();
	m_framebuffer->addTexture( "light_Position",          cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_framebuffer->addTexture( "light_Normal",            cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_framebuffer->addTexture( "light_Albedo",            cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_framebuffer->addTexture( "light_MetallicRoughness", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	m_framebuffer->addTexture( "light_Depth",             cm::TextureFormat_RGBAf, cm::TextureType_Depth );
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

	renderer.clear( 0x44A5FFFF, cm::eClearMode::ClearMode_Color | cm::ClearMode_Depth );

	iCamera* camera = cApplication::getInstance().m_current_camera;
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 proj = camera->getProjectionMatrix();

	//m_shader->ubBegin();
	//m_shader->ubBufferData( "uRenderMode",                &renderer.debug_render_mode,          sizeof( int ) );
	//m_shader->ubBufferData( "uDirectionalLight",          &dirl,                                sizeof( cVector3f ) );
	//m_shader->ubBufferData( "uAmbientLightIntensity",     &ambl_intensity,                      sizeof( float ) );
	//m_shader->ubBufferData( "uDirectionalLightIntensity", &dirl_intensity,                      sizeof( float ) );
	//m_shader->ubBufferData( "uCameraDirection",           &cam_dir,                             sizeof( cVector3f ) );
	//m_shader->ubBufferData( "uCameraPosition",            &cam_pos,                             sizeof( cVector3f ) );
	//m_shader->ubBufferData( "uNumPointLights",            &numlights,                           sizeof( int ) );
	//m_shader->ubBufferData( "uViewMatrix",                glm::value_ptr( view ),               sizeof( cVector4f ) * 4 ); // TODO: change to wv::matrix
	//m_shader->ubBufferData( "uProjMatrix",                glm::value_ptr( proj ),               sizeof( cVector4f ) * 4 ); // TODO: change to wv::matrix
	//m_shader->ubBufferData( "uLightPos[0]",               scene_manager.light_positions.data(), sizeof( cVector4f ) * numlights );
	//m_shader->ubBufferData( "uLightCol[0]",               scene_manager.light_colors.data(),    sizeof( cVector4f ) * numlights );
	//m_shader->ubEnd();
}
