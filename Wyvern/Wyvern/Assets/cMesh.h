#pragma once

#include <Wyvern/Assets/iAsset.h>
#include <Wyvern/Renderer/Framework/cIndexBuffer.h>
#include <Wyvern/Renderer/Framework/cVertexArray.h>
#include <Wyvern/Renderer/Framework/cVertexBuffer.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cMesh
	{

	public:

		 cMesh( void );
		~cMesh( void );

///////////////////////////////////////////////////////////////////////////////////////

		void create( void );
		void render( void ); // move?

///////////////////////////////////////////////////////////////////////////////////////
		
		std::vector<float> vertices = { // xyz rgb
			 0.8f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f  // top left
		};

		std::vector<unsigned int> indices = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
		
		// std::vector< float >        vertices{};
		// std::vector< unsigned int > indices {};

///////////////////////////////////////////////////////////////////////////////////////

	private:

		cVertexArray  m_vertexArray;
		cVertexBuffer m_vertexBuffer;
		cIndexBuffer  m_indexBuffer;

///////////////////////////////////////////////////////////////////////////////////////
	};

///////////////////////////////////////////////////////////////////////////////////////

}