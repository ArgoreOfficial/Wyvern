#include "cModel.h"
#include <Wyvern/Filesystem/cFilesystem.h>

using namespace WV;

/*
// ------------ TEMPORARY ------------ //
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

	bgfx::ShaderHandle handle = bgfx::createShader( Filesystem::loadMemoryFromFile( filePath ) );
	bgfx::setName( handle, _name );

	return handle;
}
*/

cModel::cModel( cMesh& _mesh )
{
	/*
	m_layout.begin()
		.add( bgfx::Attrib::Position, 3, bgfx::AttribType::Float )
		.add( bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true, true )
		//.add( bgfx::Attrib::Tangent, 4, bgfx::AttribType::Uint8, true, true )
		.add( bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true, true )
		.end();
	
	m_layout.begin()
		.add( bgfx::Attrib::Position, 3, bgfx::AttribType::Float ) // position attribute
		.add( bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true ) // colour attribute
		.end();

	auto& vertices = _mesh.getVertices();
	auto& indices = _mesh.getIndices();

	const bgfx::Memory* verts = Filesystem::vectorToMemory<Mesh::Vertex>( &vertices );
	const bgfx::Memory* inds = Filesystem::vectorToMemory<uint16_t>( &indices );

	m_vertexBufferHandle = bgfx::createVertexBuffer( verts, m_layout);
	m_indexBufferHandle = bgfx::createIndexBuffer( inds );

	m_vertexShaderHandle = loadShader( "vs_cubes" );
	m_fragmentShaderHandle = loadShader( "fs_cubes" );
	m_program = bgfx::createProgram( m_vertexShaderHandle, m_fragmentShaderHandle, true );
	*/
}

cModel::~cModel()
{
	/*
	bgfx::destroy( m_vertexBufferHandle );
	bgfx::destroy( m_indexBufferHandle );
	bgfx::destroy( m_program );
	bgfx::destroy( m_vertexShaderHandle );
	bgfx::destroy( m_fragmentShaderHandle );
	*/
}

void WV::cModel::setPosition( float _x, float _y, float _z )
{
	m_positionX = _x;
	m_positionY = _y;
	m_positionZ = _z;
}

void WV::cModel::setRotation( float _x, float _y, float _z )
{
	m_rotationX = _x;
	m_rotationY = _y;
	m_rotationZ = _z;
}

void WV::cModel::setScale( float _x, float _y, float _z )
{
	m_scaleX = _x;
	m_scaleY = _y;
	m_scaleZ = _z;
}

void WV::cModel::move( float _x, float _y, float _z )
{
	m_positionX += _x;
	m_positionY += _y;
	m_positionZ += _z;
}

void WV::cModel::rotate( float _x, float _y, float _z )
{
	m_rotationX += _x;
	m_rotationY += _y;
	m_rotationZ += _z;
}

void cModel::submit( float _aspect )
{
	/*
	float scale[ 16 ];
	bx::mtxScale( scale, m_scaleX, m_scaleY, m_scaleZ );
	
	float rot[ 16 ];
	bx::mtxRotateXYZ( rot, m_rotationX, m_rotationY, m_rotationZ );
	
	float transl[ 16 ];
	bx::mtxTranslate( transl, m_positionX, m_positionY, m_positionZ );

	float mtx[ 16 ];
	bx::mtxMul( mtx, scale, rot );
	bx::mtxMul( mtx, mtx, transl );

	bgfx::setTransform( mtx );

	bgfx::setVertexBuffer( 0, m_vertexBufferHandle );
	bgfx::setIndexBuffer( m_indexBufferHandle );

	uint64_t state = 0
		| BGFX_STATE_WRITE_R
		| BGFX_STATE_WRITE_G
		| BGFX_STATE_WRITE_B
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_CULL_CCW
		| BGFX_STATE_MSAA;
	bgfx::setState( state );

	bgfx::submit( 0, m_program );
	*/
}
