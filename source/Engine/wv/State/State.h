#pragma once

#include <wv/Decl.h>
#include <wv/Debug/Print.h>
#include <wv/Scene/Scene.h>

#include <string>
#include <unordered_map>

namespace wv
{
	class Scene;

	class State
	{
	WV_DECLARE_INTERFACE(State)

	public:

		/*
		
		onCreate -> onLoad -> loop(   update -> draw   ) -> onUnload -> onDestroy 
		
		*/

		virtual void onLoad()   = 0;
		virtual void onUnload() = 0;

		virtual void onCreate()
		{
			for ( auto& scene : m_scenes )
				scene.second->onCreate();
		}

		virtual void onDestroy()
		{
			for ( auto& scene : m_scenes )
			{
				scene.second->onDestroy();
				delete scene.second;
			}

			m_scenes.clear();
		}

		virtual inline void update( double _deltaTime )
		{
			if ( m_nextScene )
			{
				if( m_currentScene )
					m_currentScene->onUnload();
				
				m_nextScene->onLoad();

				printf( "Switched Scene\n" );
				m_currentScene = m_nextScene;
				m_nextScene = nullptr;
			}
		}
		virtual void draw( GraphicsDevice* _device ) = 0;

		inline void switchToScene( const std::string& _name )
		{
			if ( !m_scenes.count( _name ) )
			{
				Debug::Print( Debug::WV_PRINT_ERROR, "Scene '%s' does not exists\n", _name.c_str() );
				return;
			}

			printf( "Switching to scene '%s'\n", _name.c_str() );
			m_nextScene = m_scenes[ _name ];
		}

	protected:

		template<class T>
		void addScene( const std::string& _name );

		std::unordered_map<std::string, Scene*> m_scenes;

 		Scene* m_nextScene = nullptr;
		Scene* m_currentScene = nullptr;

	};

	template<class T>
	inline void State::addScene( const std::string& _name )
	{
		if ( m_scenes.contains( _name ) )
		{
			Debug::Print( Debug::WV_PRINT_ERROR, "Scene '%s' already exists\n", _name.c_str() );
			return;
		}

		Scene* scene = new T();
		m_scenes[ _name ] = scene;
	}
}