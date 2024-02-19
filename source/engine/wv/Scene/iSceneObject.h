#pragma once

#include <wv/Scene/cScene.h>
#include <wv/Scene/cSceneManager.h>
#include <string>

#include <wv/Decl.h>

namespace wv 
{

	class iSceneObject
	{
	WV_DECLARE_INTERFACE( iSceneObject )

	public:
		
		void destroy()
		{
			for ( int i = 0; i < m_children.size(); i++ )
				delete m_children[ i ];
			
			m_children.clear();
		}

		template<class T>
		T* addObject( std::string _name )
		{
			iSceneObject* scene_object = new T( _name );
			scene_object->setParent( this );

			m_children.push_back( scene_object );
		}

		virtual void update( double _delta_time ) = 0;
		virtual void render() = 0;

		void setParent( iSceneObject* _parent ) { m_parent = _parent; }

	protected:
		iSceneObject( std::string _name ) : m_name{ _name } 
		{ 
			cSceneManager::getInstance().getActiveScene()->addObject( this );
		};

		std::string m_name;
		iSceneObject* m_parent = nullptr;
		std::vector<iSceneObject*> m_children;

	};

}
