#pragma once

#include <wv/Core/iSingleton.h>

#include "wv/Math/Vector2.h"
#include "wv/Math/Vector3.h"
#include "wv/Math/Vector4.h"

#include <map>
#include <string>
#include <vector>

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

		cVector3f getDirectionalLightDirection( void );
		cVector2f getDirectionalLightRotation ( void ) { return m_directional_light_rotation; }
		
		float getDirectionalLightIntensity( void ) { return m_directional_light_intensity; }
		float getAmbientLightIntensity    ( void ) { return m_ambient_light_intensity; }

		void rotateDirectionalLight( cVector2f _rotation );

		std::vector<cVector4f> light_positions;
		std::vector<cVector4f> light_colors;

	private:

		std::map<std::string, cScene*> m_scenes;
		cScene* m_active_scene = nullptr;
		cScene* m_next_scene   = nullptr;

		float m_transition_time   = 0.0f;
		float m_transition_length = 0.0f;

		/* TODO: change to cLight */
		cVector2f m_directional_light_rotation{ -40.0f, 0.0f };
		float m_directional_light_intensity = 0.8f;

		float m_ambient_light_intensity = 0.1f;

	};

}

