#pragma once

#include "TimelineRecorder.h"

#include <wv/Reflection/Reflection.h>
#include <wv/Scene/SceneObject.h>
#include <wv/Engine/Engine.h>

#include <wv/Events/InputListener.h>

namespace psq
{
	class cRewindSettingsWindow : public wv::iSceneObject, public wv::IInputListener
	{
	public:
		 cRewindSettingsWindow( const wv::UUID& _uuid, const std::string& _name );
		~cRewindSettingsWindow() {}

		static cRewindSettingsWindow* createInstance( void ) { return nullptr; }
		static cRewindSettingsWindow* createInstanceJson( nlohmann::json& _json ) 
		{
			wv::UUID    uuid = _json.value( "uuid", wv::cEngine::getUniqueUUID() );
			std::string name = _json.value( "name", "" );

			return new cRewindSettingsWindow( uuid, name ); 
		}

		void onInputEvent( wv::InputEvent _event ) override;

		static void togglePause() { cRewindSettingsWindow::worldIsPaused ^= 1; }

		static inline bool isRewinding   = false;
		static inline bool worldIsPaused = false;
		static inline int  currentFrame  = 0;
		
		static inline int rewindSpeed = 1;
	private:
		cTimelineRecorder m_recorder;

		bool m_rewindImgui = false;
		bool m_firstFrame  = true; // because imgui weird
		int m_maxFrames = 0;

		wv::Vector2f m_windowPos{ 100.0f, 100.0f };

		// Inherited via iSceneObject
		void onLoadImpl   ( void ) override { };
		void onUnloadImpl ( void ) override { };
		void onCreateImpl ( void ) override { };
		void onDestroyImpl( void ) override { };

		void updateImpl( double _deltaTime ) override;
		void drawImpl  ( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) override;

	};

}

REFLECT_CLASS( psq::cRewindSettingsWindow );