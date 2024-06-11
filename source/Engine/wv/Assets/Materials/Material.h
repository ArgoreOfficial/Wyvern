#pragma once

#include <vector>
#include <string>

namespace wv
{
	class Pipeline;
	class GraphicsDevice;
	class Mesh;
	class Texture;

	class Material
	{
	public:
		virtual bool loadFromFile( const char* _path );
		virtual bool loadFromSource( const std::string& _source );
		virtual void destroy();

		void setAsActive( GraphicsDevice* _device );

		virtual void materialCallback();
		virtual void instanceCallback( Mesh* _instance );

	protected:
		Pipeline* m_pipeline = nullptr;
		std::vector<Texture*> m_textures;
	};
}