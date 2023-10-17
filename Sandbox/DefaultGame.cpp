#include "DefaultGame.h"
#include <Wyvern/Filesystem/Filesystem.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <bx/readerwriter.h>
#include <filesystem>

void DefaultGame::load()
{

}


static const bgfx::Memory* readMem( std::string file_path )
{
	std::ifstream ifs( file_path, std::ios::binary );

	std::vector<char> data( std::filesystem::file_size( file_path ) / sizeof( char ) );
	ifs.read( reinterpret_cast<char*>( data.data() ), data.size() * sizeof( char ) );

	uint32_t size = data.size();

	const bgfx::Memory* mem = bgfx::alloc( size + 1 );
	for ( int i = 0; i < data.size(); i++ )
	{
		mem->data[ i ] = data[ i ];
	}
	mem->data[ mem->size - 1 ] = '\0';
	return mem;
}

static const bgfx::Memory* loadMem( bx::FileReaderI* _reader, const char* _filePath )
{
	if ( bx::open( _reader, _filePath ) )
	{
		uint32_t size = (uint32_t)bx::getSize( _reader );
		const bgfx::Memory* mem = bgfx::alloc( size + 1 );
		bx::read( _reader, mem->data, size, bx::ErrorAssert{} );
		bx::close( _reader );
		mem->data[ mem->size - 1 ] = '\0';
		return mem;
	}

	WVDEBUG( "Failed to load %s.", _filePath );
	return NULL;
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

	bgfx::ShaderHandle handle = bgfx::createShader( readMem( filePath ) );
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


	// --------------- MESH TESTING --------------- //


	layout.begin()
		.add( bgfx::Attrib::Position, 3, bgfx::AttribType::Float ) // position attribute
		.add( bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true ) // colour attribute
		.end();
	vbh = bgfx::createVertexBuffer( bgfx::makeRef( cubeVertices, sizeof( cubeVertices ) ), layout );
	ibh = bgfx::createIndexBuffer( bgfx::makeRef( cubeIndices, sizeof( cubeIndices ) ) );

	vsh = loadShader( "vs_cubes" );
	fsh = loadShader( "fs_cubes" );
	program = bgfx::createProgram( vsh, fsh, true );


	// --------------- MESH TESTING --------------- //

}

void DefaultGame::update( float _deltaTime )
{
	float time = WV::Application::getTime();

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
