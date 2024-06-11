#pragma once

#include <vector>

namespace wv
{
	class Pipeline;
	class GraphicsDevice;
	class Node;
	class Texture;

	class Material
	{
	public:
		virtual bool load( const char* _path );
		virtual void destroy();

		void setAsActive( GraphicsDevice* _device );

		virtual void materialCallback();
		virtual void instanceCallback( Node* _instance );

	protected:
		Pipeline* m_pipeline = nullptr;
		std::vector<Texture*> m_textures;
	};
}