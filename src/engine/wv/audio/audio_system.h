#pragma once

#include <vector>

namespace wv {

class AudioSystem
{
	friend class Application;

public:
	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	virtual void enableMicrophone() = 0;
	virtual void disableMicrophone() = 0;

	size_t getMicrophoneSampleCount() const { return m_microphoneSampleCount; }
	float* getMicrophoneSamples()     const { return m_microphoneSamples; }
	float  getMicrophonePeak()        const { return m_microphonePeak; }

protected:

	virtual void updateRecordingDevices() = 0;

	bool m_isMicrophoneEnabled = false;

	float m_microphonePeak = 0.0f;

	float* m_microphoneSamples = nullptr;
	size_t m_microphoneSampleCount = 0; // number of float samples
	size_t m_microphoneAllocatedSampleSize = 0; // total allocated number of bytes 
};

namespace Platform {

AudioSystem* createAudioSystem();

}

}