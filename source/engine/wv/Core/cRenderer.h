#pragma once

#include <wv/Core/iSingleton.h>

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
		void onDestroy();

		void onResize( int _width, int _height );
		void clear   ( unsigned int _color, int _mode );

		void begin( void );
		void end  ( void );

		void setBackendType( cRenderer::eBackendType _backend ) { m_backend_type = _backend; }

		cm::iBackend* getBackend   ( void ) { return m_backend; }
		cFramebuffer* getGBuffer   ( void ) { return m_gbuffer; }
		cMesh*        getScreenQuad( void ) { return m_screen_quad; }

		void addRenderPass( iRenderPass* _render_pass );

		int debug_render_mode   = 1;

	private:

		eBackendType  m_backend_type = eBackendType::BackendType_OpenGL;
		cm::iBackend* m_backend      = nullptr;

		cMesh*        m_screen_quad = nullptr;
		cFramebuffer* m_gbuffer     = nullptr;

		std::vector<iRenderPass*> m_render_passes;
		iRenderPass* m_lightpass;
	};

}
