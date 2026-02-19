#ifdef WV_SUPPORT_SDL

#include <wv/audio/audio_system.h>

#include <SDL3/SDL_audio.h>

namespace wv {

class SDLAudioSystem : AudioSystem
{
	friend class Application;

public:
	virtual void initialize() override;
	virtual void shutdown() override;
	
	virtual void enableMicrophone() override;
	virtual void disableMicrophone() override;

protected:

	virtual void updateRecordingDevices() override;

	SDL_AudioStream* m_defaultRecordingStream{ nullptr };
	SDL_AudioStream* m_defaultPlaybackStream{ nullptr };

};

}

#endif