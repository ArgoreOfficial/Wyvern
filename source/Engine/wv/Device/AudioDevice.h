#pragma once

#include <miniaudio.h>

#include <wv/Audio/Audio.h>

namespace wv
{

	struct AudioDeviceDesc
	{

	};

	class AudioDevice
	{
	public:
		
		AudioDevice( AudioDeviceDesc* _desc );
		void terminate();

		Audio* loadAudio2D(const char* _path);
		Audio* loadAudio3D( const char* _path );
		void unloadAudio( Audio* _audio );

	private:

		ma_engine m_engine;

	};
}