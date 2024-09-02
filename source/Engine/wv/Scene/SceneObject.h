#pragma once

#include <wv/Math/Transform.h>
#include <wv/Types.h>

#include <stdint.h>
#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iDeviceContext;
	class iGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	class iSceneObject
	{
	public:

		 iSceneObject( const UUID& _uuid, const std::string& _name );
		~iSceneObject();

		void addChild   ( iSceneObject* _node, bool _triggerLoadAndCreate = false );
		void removeChild( iSceneObject* _node );
		void moveChild  ( iSceneObject* _node, iSceneObject* _newParent );

		std::string getName( void ) { return m_name; }
		uint64_t    getUUID( void ) { return m_uuid; }
		
	#ifdef WV_EDITOR
		bool isEditorSelected() { return m_editorSelected; }
	#endif

		std::vector<iSceneObject*> getChildren( void ) { return m_children; };
		iSceneObject*              getParent  ( void ) { return m_parent; }

		void onLoad()
		{
			if( !m_loaded )
			{
				onLoadImpl();
				m_loaded = true;
			}

			for ( size_t i = 0; i < m_children.size(); i++ )
				m_children[ i ]->onLoad();
		}
		
		void onUnload()
		{
			if( m_loaded )
			{
				onUnloadImpl();
				m_loaded = false;
			}

			for ( size_t i = 0; i < m_children.size(); i++ )
				m_children[ i ]->onUnload();
		}
		
		void onCreate()
		{
			if( !m_created )
			{
				onCreateImpl();
				m_created = true;
			}

			for ( size_t i = 0; i < m_children.size(); i++ )
				m_children[ i ]->onCreate();
		}

		void onDestroy()
		{
			if( m_created )
			{
				onDestroyImpl();
				m_created = false;
			}

			for ( size_t i = 0; i < m_children.size(); i++ )
				m_children[ i ]->onDestroy();
		}

		void update( double _deltaTime ) 
		{
			if( m_loaded && m_created )
			{
				updateImpl( _deltaTime );
				m_transform.update();
			}

			for ( size_t i = 0; i < m_children.size(); i++ )
				m_children[ i ]->update( _deltaTime );
		}

		void draw( iDeviceContext* _context, iGraphicsDevice* _device ) 
		{
			if( m_loaded && m_created )
				drawImpl( _context, _device );
			
			for ( size_t i = 0; i < m_children.size(); i++ )
				m_children[ i ]->draw( _context, _device );
		}

		Transformf m_transform;
		Transformf m_worldTransform;

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		virtual void onLoadImpl() = 0;
		virtual void onUnloadImpl() = 0;

		virtual void onCreateImpl() = 0;
		virtual void onDestroyImpl() = 0;

		virtual void updateImpl( double _deltaTime ) = 0;
		virtual void drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) = 0;

		uint64_t    m_uuid;
		std::string m_name;
		
		iSceneObject* m_parent = nullptr;
		std::vector<iSceneObject*> m_children{};

	#ifdef WV_EDITOR
		bool m_editorSelected = false;
	#endif

	private:
		bool m_loaded  = false;
		bool m_created = false;

	};

}

// REFLECTNODE( INode )