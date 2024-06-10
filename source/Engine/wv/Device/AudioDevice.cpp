#include "AudioDevice.h"

#include <wv/Debug/Print.h>
#include <string>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

wv::AudioDevice::AudioDevice( AudioDeviceDesc* _desc )
{
	ma_result res = ma_engine_init( nullptr, &m_engine );

	if ( res != MA_SUCCESS )
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Failed to initialize ma_audio\n" );
}

void wv::AudioDevice::terminate()
{

}

wv::Audio* wv::AudioDevice::loadAudio2D( const char* _path )
{
	std::string path = std::string{ "res/audio/" } + _path;
	Audio* audio = new Audio();
	ma_result res = ma_sound_init_from_file( &m_engine, "res/audio/sample.mp3", 0, nullptr, nullptr, &audio->sound);
	return audio;
}

wv::Audio* wv::AudioDevice::loadAudio3D( const char* _path )
{
	/// TODO: Implement
	return {};
}

void wv::AudioDevice::unloadAudio( Audio* _audio )
{
	ma_sound_uninit( &_audio->sound );
}
