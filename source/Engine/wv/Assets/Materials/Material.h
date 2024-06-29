#pragma once

#include <vector>
#include <string>

#include <wv/Resource/Resource.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderProgram;
	class iGraphicsDevice;
	class Mesh;
	class Texture;

///////////////////////////////////////////////////////////////////////////////////////

	class Material : public iResource
	{

	public:

		Material( std::string _name ) : 
			iResource{ _name, L"" }
		{ }

		bool loadFromFile( const char* _path );
		bool loadFromSource( const std::string& _source );
		void destroy( iGraphicsDevice* _pGraphicsDevice ) override;

		void setAsActive( iGraphicsDevice* _device );

		virtual void materialCallback();
		virtual void instanceCallback( Mesh* _instance );

		bool tempIsCreated();

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		cShaderProgram* m_program = nullptr;
		std::vector<Texture*> m_textures;

	};

}
