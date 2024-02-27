#pragma once

#include <cm/Framework/Texture.h>
#include <cm/Framework/Framebuffer.h>

#include <string>
#include <wv/Math/Vector2.h>

namespace wv 
{
    class cShader;

    class cFramebuffer 
    {
    public:
         cFramebuffer( void );
        ~cFramebuffer( void );

		void create( int _samples = 0 );
        void finalize();
        
		void addTexture( std::string _name, cm::eTextureFormat _format, cm::eTextureType _type );
		void addRenderbuffer( std::string _name, cm::eRenderbufferType _type );

		void bind();
        void bindTexturesToShader( cShader* _shader );
        void unbind();

        void onResize( int _width, int _height );

		wv::cVector2i getSize() { return m_size; }
		cm::sFramebuffer m_framebuffer_object;
		
	private:
		
		wv::cVector2i m_size = { 1, 1 };

    };
}