#include "AudioDevice.h"

#include <wv/Application/Application.h>
#include <wv/Debug/Print.h>
#include <string>

#ifdef WV_PLATFORM_WASM
//#define MA_AUDIO_WORKLETS_THREAD_STACK_SIZE 81920
#include <emscripten/webaudio.h>
#endif

#define MINIAUDIO_IMPLEMENTATION
#include <wv/Auxiliary/miniaudio.h>

wv::AudioDevice::AudioDevice( AudioDeviceDesc* _desc )
{
	m_engine = new ma_engine();
	initialize();
}

void wv::InternalAudio::onNotif( const ma_device_notification* pNotification )
{
	if ( pNotification->type == ma_device_notification_type::ma_device_notification_type_unlocked )
		wv::Application::get()->audio->m_unlocked = true;
}

void wv::AudioDevice::initialize()
{
	ma_engine_config config = ma_engine_config_init();
	config.notificationCallback = InternalAudio::onNotif;
	config.listenerCount = 1;

	ma_result res = ma_engine_init( &config, m_engine );

	if ( res != MA_SUCCESS )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Failed to initialize ma_audio\n" );
		return;
	}

	m_enabled = true;

#ifndef WV_PLATFORM_WASM
	m_unlocked = true;
#endif
}

void wv::AudioDevice::terminate()
{
	m_enabled = false;
	ma_engine_uninit( m_engine );
}

wv::Audio* wv::AudioDevice::loadAudio2D( const char* _path )
{
	if( !m_enabled )
		return nullptr;

	Audio* audio = loadAudio3D( _path );
	
	ma_sound_set_positioning( &audio->sound, ma_positioning_relative );
	ma_sound_set_position( &audio->sound, 0.0f, 0.0f, 0.0f );

	return audio;
}

wv::Audio* wv::AudioDevice::loadAudio3D( const char* _path )
{
	if ( !m_enabled )
		return nullptr;

	std::string path = std::string{ "res/audio/" } + _path;
	Audio* audio = new Audio();
	
	ma_result res = ma_sound_init_from_file( m_engine, path.c_str(), MA_SOUND_FLAG_DECODE, nullptr, nullptr, &audio->sound );
	
	if ( res != MA_SUCCESS )
		Debug::Print( Debug::WV_PRINT_ERROR, "Failed to init Audio '%s'\n", _path );
	else
		Debug::Print( Debug::WV_PRINT_DEBUG, "Created Audio '%s'\n", _path );

	ma_sound_set_positioning( &audio->sound, ma_positioning_absolute );

	return audio;
}

void wv::AudioDevice::unloadAudio( Audio* _audio )
{
	if( !m_enabled )
		return;

	ma_sound_uninit( &_audio->sound );
}

void wv::AudioDevice::setListenerPosition( Vector3f _position )
{
	ma_engine_listener_set_position( m_engine, 0, _position.x, _position.y, _position.z );
}

void wv::AudioDevice::setListenerDirection( Vector3f _direction )
{
	ma_engine_listener_set_direction( m_engine, 0, _direction.x, _direction.y, _direction.z );
}
