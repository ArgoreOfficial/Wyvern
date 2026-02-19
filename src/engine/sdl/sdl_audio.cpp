#ifdef WV_SUPPORT_SDL

#include "sdl_audio.h"

#include <wv/memory/memory.h>
#include <wv/math/math.h>

namespace wv {

void SDLAudioSystem::initialize()
{

}

void SDLAudioSystem::shutdown()
{
	disableMicrophone();
}

void SDLAudioSystem::enableMicrophone()
{
	if ( m_isMicrophoneEnabled )
		return;

	SDL_AudioSpec audioSpec{};
	audioSpec.freq = 44100;
	audioSpec.format = SDL_AUDIO_F32;
	audioSpec.channels = 2;

	m_defaultRecordingStream = SDL_OpenAudioDeviceStream( SDL_AUDIO_DEVICE_DEFAULT_RECORDING, &audioSpec, nullptr, nullptr );
	
	if ( m_defaultRecordingStream == nullptr )
	{
		wv::Debug::Print( "[SDL] %s\n", SDL_GetError() );
		return;
	}
	
	SDL_ResumeAudioStreamDevice( m_defaultRecordingStream );
	m_isMicrophoneEnabled = true;
}

void SDLAudioSystem::disableMicrophone()
{
	if ( !m_isMicrophoneEnabled )
		return;

	SDL_DestroyAudioStream( m_defaultRecordingStream );
	m_defaultRecordingStream = nullptr;

	if ( m_microphoneSamples )
	{
		WV_FREE_ARR( m_microphoneSamples );
		m_microphoneSamples = nullptr;
	}
	
	m_microphoneAllocatedSampleSize = 0;
	m_microphonePeak = 0.0f;
	m_microphoneSampleCount = 0;

	m_isMicrophoneEnabled = false;
}

void SDLAudioSystem::updateRecordingDevices()
{
	if ( m_defaultRecordingStream )
	{
		int availableStreamBytes = SDL_GetAudioStreamAvailable( m_defaultRecordingStream );

		if ( availableStreamBytes > 0 )
		{
			// reallocate buffer if it's too small
			if ( m_microphoneAllocatedSampleSize < availableStreamBytes )
			{
				if ( m_microphoneSamples )
					WV_FREE_ARR( m_microphoneSamples );

				m_microphoneSamples = reinterpret_cast<float*>( WV_NEW_ARR( uint8_t, availableStreamBytes ) );
				m_microphoneAllocatedSampleSize = availableStreamBytes;
			}

			m_microphoneSampleCount = SDL_GetAudioStreamData( m_defaultRecordingStream, m_microphoneSamples, availableStreamBytes );

			float truePeak = 0.0f;
			size_t numSamples = m_microphoneSampleCount / sizeof( float );
			for ( size_t i = 0; i < numSamples; i++ )
				truePeak += wv::Math::abs( m_microphoneSamples[ i ] );

			m_microphonePeak = truePeak / numSamples;
		}
	}
}

namespace Platform {

AudioSystem* createAudioSystem() {
	return (AudioSystem*)WV_NEW( SDLAudioSystem );
}

}

}

#endif