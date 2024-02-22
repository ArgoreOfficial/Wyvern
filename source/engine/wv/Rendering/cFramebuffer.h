#pragma once

#include <cm/Framework/Texture.h>
#include <cm/Framework/Framebuffer.h>

#include <string>

namespace wv 
{
    class cShader;

    class cFramebuffer 
    {
    public:
         cFramebuffer( void );
        ~cFramebuffer( void );

		void create();
        void finalize();
        
		void addTexture( std::string _name, cm::eTextureFormat _format, cm::eTextureType _type );
        
		void bind();
        void bindTexturesToShader( cShader* _shader );
        void unbind();

        void onResize();

		cm::sFramebuffer m_framebuffer_object;
    private:


    };
}