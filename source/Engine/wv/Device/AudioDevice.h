#pragma once

#include <wv/Audio/Audio.h>

namespace wv
{

	struct AudioDeviceDesc
	{

	};

	namespace InternalAudio
	{
		void onNotif( const ma_device_notification* pNotification );
	}

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
	private:

		friend void InternalAudio::onNotif( const ma_device_notification* pNotification );

		ma_engine* m_engine;
		bool m_enabled = false;
		bool m_unlocked = false;

	};
}