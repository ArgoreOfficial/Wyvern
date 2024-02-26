#pragma once

#include <wv/Core/iSingleton.h>
#include <wv/Math/Vector2.h>

#include <vector>

namespace cm { class iBackend; }
namespace cm { class iShader; }

namespace wv
{
    class iRenderPass;
	class cMesh;
	class cShader;
	class cFramebuffer;
	class cAssemblerPass;

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

		void onResize( int _width, int _height ); // TODO: change to iWindowEvent
		void clear   ( unsigned int _color, int _mode );

		cFramebuffer* getViewbuffer    ( void ) { return m_viewbuffer; }
		wv::cVector2i getViewportSize  ( void ) { return m_viewport_size; }
		int           getViewportWidth ( void ) { return m_viewport_size.x; }
		int           getViewportHeight( void ) { return m_viewport_size.y; }
		float         getViewportAspect( void );

		void begin( void );
		void end  ( void );
		void display( void );

		void setBackendType( cRenderer::eBackendType _backend ) { m_backend_type = _backend; }

		cm::iBackend* getBackend   ( void ) { return m_backend; }
		cFramebuffer* getGBuffer   ( void ) { return m_gbuffer; }
		cMesh*        getScreenQuad( void ) { return m_screen_quad; }

		void addRenderPass( iRenderPass* _render_pass );

		int debug_render_mode = 0;

	private:

		void createBackend( void );
		void initImgui    ( void );
		void deinitImgui  ( void );

		void onResizeInternal();

		eBackendType  m_backend_type = eBackendType::BackendType_OpenGL;
		cm::iBackend* m_backend      = nullptr;

		cMesh*        m_screen_quad = nullptr;
		cFramebuffer* m_gbuffer     = nullptr;
		cFramebuffer* m_viewbuffer  = nullptr;

		std::vector<iRenderPass*> m_render_passes;
		cAssemblerPass* m_assembler = nullptr;

		wv::cVector2i m_viewport_size;
		wv::cVector2i m_next_viewport_size;
	};

}
