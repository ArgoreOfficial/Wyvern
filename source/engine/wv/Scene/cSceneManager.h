#pragma once

#include <wv/Core/iSingleton.h>

#include <map>
#include <string>
#include <cm/Core/stb_image.h>

namespace wv
{
	class cScene;
	class cSceneLoader;

	class cSceneManager : public iSingleton<cSceneManager>
	{
	public:
		 cSceneManager( void );
		~cSceneManager( void );

		void create( void ) override;
		void update( double _delta_time );
		void render( void );

		void createScene( std::string _name );
		void switchScene( std::string _scene );

		void loadScene( cSceneLoader* _scene_loader );

		cScene* getActiveScene() { return m_active_scene; }

	private:

		std::map<std::string, cScene*> m_scenes;
		cScene* m_active_scene = nullptr;
		cScene* m_next_scene   = nullptr;

		float m_transition_time   = 0.0f;
		float m_transition_length = 0.0f;


	};

}

