#include "Mesh.h"
#include <wv/Application/Application.h>
#include <wv/Assets/Materials/IMaterial.h>
#include <wv/Device/Context.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Primitive.h>
#include <wv/Memory/MemoryDevice.h>

#include <fstream>

wv::Mesh::Mesh( const uint64_t& _uuid, const std::string& _name ) : Node{ _uuid, _name }
{

}

wv::Mesh::~Mesh()
{
	/// TODO: better cleanup
	wv::Application::get()->device->destroyPrimitive( &m_primitive ); // ew

}

void wv::Mesh::loadFromFile( const std::string& _path )
{
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

	wv::Application* app = wv::Application::get();
	Memory mem = app->memoryDevice->loadFromFile( _path.c_str() );

	wv::PrimitiveDesc prDesc;
	{
		int numIndices = *reinterpret_cast<int*>( mem.data );
		int numVertices = *reinterpret_cast<int*>( mem.data + sizeof( int ) );
		int vertsSize = numVertices * sizeof( float ) * 5; // 5 floats per vertex
		int indsSize = numIndices * sizeof( unsigned int );

		char* indexBuffer = mem.data + ( sizeof( int ) * 2 );
		char* vertexBuffer = indexBuffer + indsSize;

		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;

		prDesc.vertexBuffer = vertexBuffer;
		prDesc.vertexBufferSize = vertsSize;
		prDesc.numVertices = numVertices;

		prDesc.indexBuffer = nullptr;
		prDesc.indexBufferSize = 0;
		prDesc.numIndices = 0;
	}

	m_primitive = app->device->createPrimitive( &prDesc );
}

void wv::Mesh::update( double _deltaTime )
{
	Node::update( _deltaTime ); // update children
}

void wv::Mesh::draw( Context* _context, GraphicsDevice* _device )
{
	m_material->setAsActive( _device ); /// TODO: somewhere else

	m_material->instanceCallback( this );
	_device->draw( m_primitive );

	Node::draw( _context, _device ); // draw chldren
}
