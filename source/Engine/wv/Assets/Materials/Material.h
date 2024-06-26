#pragma once

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderProgram;
	class iGraphicsDevice;
	class Mesh;
	class Texture;

///////////////////////////////////////////////////////////////////////////////////////

	class Material
	{

	public:

		bool loadFromFile( const char* _path );
		bool loadFromSource( const std::string& _source );
		virtual void destroy();

		void setAsActive( iGraphicsDevice* _device );

		virtual void materialCallback();
		virtual void instanceCallback( Mesh* _instance );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		cShaderProgram* m_program = nullptr;
		std::vector<Texture*> m_textures;

	};

}
