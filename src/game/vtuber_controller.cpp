#include "vtuber_controller.h"

#include <wv/application.h>
#include <wv/audio/audio_system.h>
#include <wv/entity/world.h>

void VtuberControllerSystem::initialize()
{
	wv::getApp()->getAudioSystem()->enableMicrophone();
}

void VtuberControllerSystem::shutdown()
{
	wv::getApp()->getAudioSystem()->disableMicrophone();
}

void VtuberControllerSystem::update( wv::WorldUpdateContext& _ctx )
{
	wv::AudioSystem* audioSystem = wv::getApp()->getAudioSystem();

	const float threshold = 0.5f;

	float micpeak = audioSystem->getMicrophonePeak();

	micpeak *= 100.0f;
	if ( micpeak < threshold )
		micpeak = 0.0f;

	m_entity->getTransform().position.y = micpeak;
	m_entity->getTransform().update( nullptr, true );
}
