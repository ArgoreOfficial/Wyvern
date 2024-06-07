#include "Model.h"
#include <wv/Application/Application.h>
#include <wv/Device/Context.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Assets/Materials/Material.h>
#include <fstream>

wv::Model::Model( const uint64_t& _uuid, const std::string& _name ) : Node{ _uuid, _name }
{

}

wv::Model::~Model()
{
	/// TODO: better cleanup
	// wv::Application::get()->device->destroyPrimitive( &m_primitive ); // ew

}

void wv::Model::loadMemory( const std::string& _path )
{
	wv::Application* app = wv::Application::get();

	MeshDesc meshDesc;
	m_mesh = app->device->createMesh( &meshDesc );

	wv::InputLayoutElement elements[] = {
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 },
			{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }
	};

	wv::InputLayout layout;
	layout.elements = elements;
	layout.numElements = 2;

	// custom mesh format export
	/*
	// data
	int pnumIndices = 0;
	int pnumVertices = 36;

	std::ofstream cubefile( "res/cube.wpr" );
	// header
	cubefile.write( (char*)&pnumIndices, sizeof( int ) );
	cubefile.write( (char*)&pnumVertices, sizeof( int ) );
	// data
	cubefile.write( (char*)indices, sizeof( indices ) );
	cubefile.write( (char*)skyboxVertices, sizeof( skyboxVertices ) );
	cubefile.close();
	*/

	MemoryDevice md;
	Memory* mem = md.loadMemory( _path.c_str() );

	wv::PrimitiveDesc prDesc;
	{
		int numIndices = *reinterpret_cast<int*>( mem->data );
		int numVertices = *reinterpret_cast<int*>( mem->data + sizeof( int ) );
		int vertsSize = numVertices * sizeof( float ) * 5; // 5 floats per vertex
		int indsSize = numIndices * sizeof( unsigned int );

		uint8_t* indexBuffer = mem->data + ( sizeof( int ) * 2 );
		uint8_t* vertexBuffer = indexBuffer + indsSize;

		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;

		prDesc.vertexBuffer = vertexBuffer;
		prDesc.vertexBufferSize = vertsSize;
		prDesc.numVertices = numVertices;

		prDesc.indexBuffer = nullptr;
		prDesc.indexBufferSize = 0;
		prDesc.numIndices = 0;
	}

	app->device->createPrimitive( &prDesc, m_mesh );
	md.unloadMemory( mem );
}

void wv::Model::update( double _deltaTime )
{
	Node::update( _deltaTime ); // update children
}

void wv::Model::draw( Context* _context, GraphicsDevice* _device )
{
	m_material->setAsActive( _device ); /// TODO: somewhere else

	m_material->instanceCallback( this );
	_device->draw( m_mesh );

	Node::draw( _context, _device ); // draw chldren
}
