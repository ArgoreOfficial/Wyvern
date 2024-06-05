#pragma once
#include <wv/Decl.h>

namespace wv 
{
    class Pipeline;
	class GraphicsDevice;
	class Node;

    class IMaterial
    {
		WV_DECLARE_INTERFACE( IMaterial )

	public:
		virtual void create( GraphicsDevice* _device ) = 0;
		virtual void destroy( GraphicsDevice* _device ) = 0;

		void setAsActive( GraphicsDevice* _device );

		virtual void materialCallback() { }
		virtual void instanceCallback( Node* _instance ) { }
        Pipeline* m_pipeline = nullptr;
    protected:
    };
}