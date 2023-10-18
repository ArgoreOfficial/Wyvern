#include "DefaultGame.h"
#include <Wyvern/Filesystem/Filesystem.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <bx/readerwriter.h>
#include <filesystem>
#include <Wyvern/Managers/AssetManager.h>

void DefaultGame::load()
{
	WV::cAssetManager::load( "assets/beemovie.txt", WV::eAssetType::RAW );
	WV::cAssetManager::load( "assets/MULLEBAT.iso", WV::RAW );
	WV::Application::getWindow()->setClearColour( 0x222233FF );
}


static bgfx::ShaderHandle loadShader( const char* _name )
{
	char filePath[ 512 ];

	const char* shaderPath = "???";

	switch ( bgfx::getRendererType() )
	{
	case bgfx::RendererType::Noop:
	case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
	case bgfx::RendererType::Agc:
	case bgfx::RendererType::Gnm:        shaderPath = "shaders/pssl/";  break;
	case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
	case bgfx::RendererType::Nvn:        shaderPath = "shaders/nvn/";   break;
	case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
	case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
	case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;
	case bgfx::RendererType::WebGPU:     shaderPath = "shaders/spirv/"; break;

	case bgfx::RendererType::Count:
		BX_ASSERT( false, "You should not be here!" );
		break;
	}

	bx::strCopy( filePath, BX_COUNTOF( filePath ), shaderPath );
	bx::strCat( filePath, BX_COUNTOF( filePath ), _name );
	bx::strCat( filePath, BX_COUNTOF( filePath ), ".bin" );

	bgfx::ShaderHandle handle = bgfx::createShader( WV::Filesystem::loadMemoryFromFile( filePath ) );
	bgfx::setName( handle, _name );

	return handle;
}

struct PosColorVertex
{
	float x;
	float y;
	float z;
	uint32_t abgr;
};

static PosColorVertex cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeIndices[] =
{
	0, 1, 2,
	1, 3, 2,
	4, 6, 5,
	5, 6, 7,
	0, 2, 4,
	4, 2, 6,
	1, 5, 3,
	5, 7, 3,
	0, 4, 1,
	4, 5, 1,
	2, 3, 6,
	6, 3, 7,
};

void DefaultGame::start()
{
	WV::Events::KeyDownEvent::hook<DefaultGame>( &DefaultGame::handleKeyInput, this );
	WV::Application::getWindow()->setClearColour( 0x666699FF );

	// --------------- MESH TESTING --------------- //


	layout.begin()
		.add( bgfx::Attrib::Position, 3, bgfx::AttribType::Float ) // position attribute
		.add( bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true ) // colour attribute
		.end();
	vbh = bgfx::createVertexBuffer( bgfx::makeRef( cubeVertices, sizeof( cubeVertices ) ), layout );
	ibh = bgfx::createIndexBuffer( bgfx::makeRef( cubeIndices, sizeof( cubeIndices ) ) );

	vsh = loadShader( "vs_mesh" );
	fsh = loadShader( "fs_mesh" );
	program = bgfx::createProgram( vsh, fsh, true );


	// --------------- MESH TESTING --------------- //

}

void DefaultGame::update( float _deltaTime )
{
	float time = WV::Application::getTime();
	m_rotX += _deltaTime * m_slider;
	m_rotY += _deltaTime * m_slider;
}


void DefaultGame::draw()
{
	uint64_t state = 0
		| BGFX_STATE_WRITE_R
		| BGFX_STATE_WRITE_G
		| BGFX_STATE_WRITE_B
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_CULL_CW
		| BGFX_STATE_MSAA
		| BGFX_STATE_PT_TRISTRIP
		;

	const bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
	const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
	
	float view[ 16 ];
	bx::mtxLookAt( view, eye, at );
	
	float proj[ 16 ];
	bx::mtxProj( proj, 60.0f, float( 512 ) / float( 512 ), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth );
	
	bgfx::setViewTransform( 0, view, proj );
	float mtx[ 16 ];
	bx::mtxRotateXY( mtx, m_rotX, m_rotY );
	bgfx::setTransform( mtx );

	bgfx::setVertexBuffer( 0, vbh );
	bgfx::setIndexBuffer( ibh );


	bgfx::submit( 0, program );
}

void DefaultGame::drawUI()
{
	if ( m_showWindow )
	{
		if ( ImGui::Begin( "Wyvern" ) )
		{
			ImGui::Text( "Welcome to Wyvern!" );

			ImGui::SliderFloat( "I'm a Slider!", &m_slider, 0.0f, 5.0f );
		}
		ImGui::End();
	}
}

void DefaultGame::handleKeyInput( WV::Events::KeyDownEvent& _event )
{
	if ( _event.getKeyCode() == 'B' )
	{
		m_showWindow = !m_showWindow;
	}
}
