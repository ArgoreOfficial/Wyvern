#pragma once

#include <wv/audio/audio.h>
#include <wv/math/vector3.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////


	namespace InternalAudio
	{
	#ifdef WV_SUPPORT_MINIAUDIO
		void onNotif( const ma_device_notification* pNotification );
	#endif
	}

///////////////////////////////////////////////////////////////////////////////////////

	struct AudioDeviceDesc
	{

	};

///////////////////////////////////////////////////////////////////////////////////////

	class AudioDevice
	{

	public:
		
		AudioDevice( AudioDeviceDesc* _desc );
		void initialize();
		void terminate();

		Audio* loadAudio2D(const char* _path);
		Audio* loadAudio3D( const char* _path );
		void unloadAudio( Audio* _audio );

		bool isEnabled() { return m_enabled; }
		bool isUnlocked() { return m_unlocked; }

		void setListenerPosition( Vector3f _position );
		void setListenerDirection( Vector3f _direction );

///////////////////////////////////////////////////////////////////////////////////////

	private:

	#ifdef WV_SUPPORT_MINIAUDIO
		friend void InternalAudio::onNotif( const ma_device_notification* pNotification );

		ma_engine* m_engine;
	#endif
		bool m_enabled = false;
		bool m_unlocked = false;

	};

}