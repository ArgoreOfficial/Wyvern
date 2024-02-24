#include "cAssemblerPass.h"

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


void wv::cAssemblerPass::onCreate( void )
{
	m_shader = cContentManager::getInstance().getShader( "res/shaders/deferred/s_assembler" );

	m_framebuffer = new cFramebuffer();
	m_framebuffer->create();
	//m_framebuffer->addTexture( "final_Position",          cm::TextureFormat_RGBAf, cm::TextureType_Color );
	//m_framebuffer->addTexture( "final_Normal",            cm::TextureFormat_RGBAf, cm::TextureType_Color );
	//m_framebuffer->addTexture( "final_Albedo",            cm::TextureFormat_RGBAf, cm::TextureType_Color );
	//m_framebuffer->addTexture( "final_MetallicRoughness", cm::TextureFormat_RGBAf, cm::TextureType_Color );
	//m_framebuffer->addTexture( "final_Depth",             cm::TextureFormat_RGBAf, cm::TextureType_Depth );
	m_framebuffer->finalize();
}

void wv::cAssemblerPass::onDestroy( void )
{
	delete m_framebuffer;
}

void wv::cAssemblerPass::execute( cFramebuffer* _input_buffer )
{
	cRenderer& renderer = cRenderer::getInstance();
	cSceneManager& scene_manager = cSceneManager::getInstance();
	cApplication& application = cApplication::getInstance();
	cm::iBackend* backend = renderer.getBackend();

	cVector3f cam_dir = application.m_current_camera->getViewDirection();
	cVector3f cam_pos = application.m_current_camera->getTransform().position;

	cVector3f dirl = scene_manager.getDirectionalLightDirection();
	dirl.normalize();
	float dirl_intensity = scene_manager.getDirectionalLightIntensity();
	float ambl_intensity = scene_manager.getAmbientLightIntensity();
	int   numlights = scene_manager.light_positions.size();

	backend->useShaderProgram( m_shader->shader_program_handle );
	_input_buffer->bindTexturesToShader( m_shader );

	renderer.clear( 0x44A5FFFF, cm::eClearMode::ClearMode_Color | cm::ClearMode_Depth );

	iCamera* camera = cApplication::getInstance().m_current_camera;
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 proj = camera->getProjectionMatrix();

	m_shader->ubBegin();
	m_shader->ubBufferData( "uRenderMode", &renderer.debug_render_mode, sizeof( int ) );
	m_shader->ubEnd();
}
