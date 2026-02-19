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
	const float interpolationTime = 0.2f;
	const float interpolationSpeed = 1.0f / interpolationTime;

	float micpeak = audioSystem->getMicrophonePeak();

	micpeak *= 100.0f;
	if ( micpeak < threshold )
		micpeak = 0.0f;

	m_time += _ctx.deltaTime;
	
	if ( micpeak > 0.0f )
	{
		if ( m_interpolation < 1.0f )
			m_interpolation += _ctx.deltaTime * interpolationSpeed;
	}
	else
	{
		if ( m_interpolation > 0.0f )
			m_interpolation -= _ctx.deltaTime * interpolationSpeed;
	}

	if ( m_interpolation > 0.0f )
		m_talkingTime += _ctx.deltaTime;
	else
		m_talkingTime = 0.0f;

	m_interpolation = wv::Math::clamp( m_interpolation, 0.0f, 1.0f );

	wv::Vector3f idlePosition = { 0.0f, 0.0f, 0.0f };
	wv::Vector3f idleScale    = { 1.0f, 1.0f, 1.0f };
	
	wv::Vector3f talkPosition = { 0.0f, 0.0f, 0.0f };
	wv::Vector3f talkScale    = { 1.0f, 1.0f, 1.0f };

	// idle 
	{
		float bobbing = std::sin( m_time );

		idlePosition = {
			0.0f,
			bobbing * 0.3f,
			0.0f
		};
	}

	// talking
	{
		float bobbing = std::sin( m_talkingTime * 20.0f );
		float squishV = 0.5f * std::cos( m_talkingTime * 30.0f ) + 0.5f;
		float squishH = 0.5f * std::sin( m_talkingTime * 30.0f ) + 0.5f;

		squishV = 1.0f + squishV * 0.2f;
		squishH = 1.0f + squishH * 0.2f;

		talkPosition = {
			0.0f,
			bobbing * 0.6f,
			0.0f
		};

		talkScale = {
			squishV,
			squishH,
			squishV
		};
	}

	m_entity->getTransform().position = wv::Math::lerp( idlePosition, talkPosition, m_interpolation );
	m_entity->getTransform().scale    = wv::Math::lerp( idleScale,    talkScale,    m_interpolation );

	m_entity->getTransform().update( nullptr, true );
}
