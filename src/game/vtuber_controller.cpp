#include "vtuber_controller.h"

#include <wv/application.h>
#include <wv/audio/audio_system.h>
#include <wv/entity/world.h>

#include <imgui/imgui.h>

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

	const float interpolationInSpeed = 1.0f / m_interpolateInTime;
	const float interpolationOutSpeed = 1.0f / m_interpolateOutTime;

	float micpeak = audioSystem->getMicrophonePeak();

	micpeak *= 100.0f;
	if ( micpeak < threshold )
		micpeak = 0.0f;

	m_time += _ctx.deltaTime;
	
	if ( micpeak > 0.0f )
	{
		if ( m_interpolation < 1.0f )
			m_interpolation += _ctx.deltaTime * interpolationInSpeed;
	}
	else
	{
		if ( m_interpolation > 0.0f )
			m_interpolation -= _ctx.deltaTime * interpolationOutSpeed;
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
		float squish = m_yVelocity * m_interpolation;
		float squishV = 1.0f - squish;
		float squishH = 1.0f + squish;

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

	wv::Vector3f newPos = wv::Math::lerp( idlePosition, talkPosition, m_interpolation );
	m_yVelocity = ( newPos - m_entity->getTransform().position ).y;

	m_entity->getTransform().position = newPos;
	m_entity->getTransform().scale    = talkScale;

	m_entity->getTransform().update( nullptr, true );
}

void VtuberControllerSystem::onDebugRender()
{
	if ( ImGui::Begin( "Vtuber Controller System", nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		ImGui::DragFloat( "Interpolate In Time", &m_interpolateInTime, 0.025f, 0.0001f, 100.0f );
		ImGui::DragFloat( "Interpolate Out Time", &m_interpolateOutTime, 0.025f, 0.0001f, 100.0f );

		ImGui::Text( "Interpolation: %f", m_interpolation );
	}
	ImGui::End();
}
