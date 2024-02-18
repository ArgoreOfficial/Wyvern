#pragma once

#include <wv/Core/iSingleton.h>

#include <cm/Framework/Shader.h>
#include <cm/Framework/Buffer.h>

#include <vector>

namespace cm { class iBackend; }
namespace cm { class iShader; }

namespace wv
{
    class iRenderPass;
	class cMesh;
	class cShader;
	class cFramebuffer;

	class cRenderer : public iSingleton<cRenderer>
	{
	public:
		enum eBackendType
		{
			BackendType_OpenGL,
			BackendType_Direct3D11
		};

		 cRenderer( void );
		~cRenderer( void );

		void create( void ) override;
		
		void onResize( int _width, int _height );
		void clear   ( unsigned int _color, int _mode );

		void begin( void );
		void end  ( void );

		void setBackendType( cRenderer::eBackendType _backend ) { m_backend_type = _backend; }

		cm::iBackend* getBackend( void ) { return m_backend; }

		void addRenderPass( iRenderPass* _render_pass );

	private:

		eBackendType  m_backend_type = eBackendType::BackendType_OpenGL;
		cm::iBackend* m_backend      = nullptr;

		std::vector<iRenderPass*> m_render_passes;

		cMesh*           m_screen_quad;

		cFramebuffer* m_gbuffer;
		cFramebuffer* m_2buffer;

		cShader* m_gpass_shader;
		cShader* m_lightpass_shader;
		
		cShader* m_screen_shader;
		cShader* m_screen_shader2;

		cShader* m_2pass;
	};

}
